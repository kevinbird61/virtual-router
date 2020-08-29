#include "process_l3_pkt.h"
#include "debugger.h"

int 
process_ip_pkt(struct work_thrd_ctx_t *sbuff)
{
    port_t *port = sbuff->ports[sbuff->port_idx], *target_port = NULL;
    u16 port_idx = sbuff->port_idx;
    ethhdr *eth = (ethhdr *) (sbuff->pkt_buff);
    struct ipv4hdr *iph = (struct ipv4hdr *) (sbuff->pkt_buff + sbuff->nh);
    char src_ip_str[INET_ADDRSTRLEN], dst_ip_str[INET_ADDRSTRLEN];
    int nwrite = 0;
    sbuff->h = sbuff->nh + iph->ihl*4;
   
    // TODO: ttl decrement

    if (sbuff->debug) { LOG_PKT_INFO(sbuff, LOG_IP); }

    // TODO: IP options

    // check if this packet is for us
    if(iph->daddr != port->ip_addr){
        // TODO: using routing table 
        // - lookup route
        target_port = lookup_route(iph->daddr);
        if(target_port == NULL){
            // - if not found, then drop 
        } else {
            // send to target 
            memcpy(eth->smac, target_port->mac, 6);
            if(fill_dmac_by_arp(iph->daddr, eth->dmac) >= 0){
                if( pkt_send(target_port, sbuff->pkt_buff, sbuff->nh + ntohs(iph->tot_len)) >= 0 ){
                    STATS_INC_SENT_PKT(sbuff);
                }
            } else {
                // send arp
                u32 target_ip = iph->daddr;
                arphdr *arph = (arphdr *) (sbuff->pkt_buff + sbuff->nh);
                memset(sbuff->pkt_buff, 0x00, 1500);
                memcpy(eth->smac, target_port->mac, 6);
                memcpy(eth->dmac, bcast_mac, 6);
                eth->ethertype = htons(ETH_ARP);
                arph->arp_hard_type = htons(0x0001); // ethernet
                arph->arp_proto_type = htons(0x0800); // ipv4
                arph->arp_hard_size = 6;
                arph->arp_proto_size = 4;
                arph->arp_ip_daddr = target_ip;
                arph->arp_ip_saddr = target_port->ip_addr;
                memcpy(arph->arp_eth_src, target_port->mac, 6);
                arph->arp_op = htons(ARP_OP_REQ);
                
                add_new_arp_cache(sbuff);

                nwrite = pkt_send(target_port, sbuff->pkt_buff, sizeof(ethhdr) + sizeof(arphdr));
                if (nwrite > 0){
                    if (sbuff->debug){
                        LOG_TO_SCREEN("(%d)[%04d] Router send ARP request", port_idx, sbuff->sent_pkts);
                        LOG_TO_SCREEN("(%d)[%04d] Send packet to port:%d (%d bytes)", port->idx, sbuff->sent_pkts, target_port->idx, nwrite);
                    }
                    STATS_INC_SENT_PKT(sbuff);
                }
            }
        }
        

        return 1;
    } 

    // the packet is for us, do further processing
    switch(iph->proto){
        case IPPROTO_ICMP:
            process_icmp_pkt(sbuff);
            break;
        case IPPROTO_TCP:
            process_tcp_pkt(sbuff);
            break;
        case IPPROTO_UDP:
            process_udp_pkt(sbuff);
            break;
    }
}

int
process_arp_pkt(struct work_thrd_ctx_t *sbuff)
{
    port_t *port = sbuff->ports[sbuff->port_idx];
    u16 port_idx = sbuff->port_idx;
    ethhdr *eth = (ethhdr *) (sbuff->pkt_buff);
    arphdr *arph = (arphdr *) (sbuff->pkt_buff + sbuff->nh);
    char arp_src_ip_str[INET_ADDRSTRLEN], arp_dst_ip_str[INET_ADDRSTRLEN];
    u8  found = 0, entry_idx = 0;
    u16 arp_op = ntohs(arph->arp_op);
    u16 nwrite = 0;

    inet_ntop(AF_INET, &arph->arp_ip_saddr, arp_src_ip_str, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &arph->arp_ip_daddr, arp_dst_ip_str, INET_ADDRSTRLEN);

    LOG_PKT_INFO(sbuff, LOG_ARP); 
    
    switch(arp_op){
        case ARP_OP_REQ:
            // check arp target is us or not
            if( arph->arp_ip_daddr == port->ip_addr ) {
                // modify arp header to arp reply
                arph->arp_ip_daddr = arph->arp_ip_saddr;
                arph->arp_ip_saddr = port->ip_addr;
                memcpy(arph->arp_eth_src, port->mac, 6);
                memcpy(arph->arp_eth_dst, eth->smac, 6);
                memcpy(eth->dmac, eth->smac, 6);
                memcpy(eth->smac, port->mac, 6);
                arph->arp_op = htons(ARP_OP_RES);
                // send the arp reply back (unicast)
                nwrite = pkt_send(port, sbuff->pkt_buff, sbuff->nh + sizeof(arphdr));

                if (nwrite > 0) {
                    if (sbuff->debug) {
                        LOG_TO_SCREEN("(%d)[%04d] Send ARP reply back to %s", port_idx, sbuff->sent_pkts, arp_src_ip_str);
                        LOG_TO_SCREEN("(%d)[%04d] Send packet to port:%d (%d bytes)", port->idx, sbuff->sent_pkts, port->idx, nwrite);
                    }
                    STATS_INC_SENT_PKT(sbuff);
                }
            } else {
                // drop 
                if(sbuff->debug) {
                    LOG_TO_SCREEN("(%d)[%04d] ARP request is not for us, drop it", port_idx, sbuff->sent_pkts);
                }
                STATS_INC_DROP_PKT(sbuff);
            }

            break;
        case ARP_OP_RES:
            // check arp target is us or not
            if( arph->arp_ip_daddr == port->ip_addr ) {
                resolve_arp_cache(sbuff);
            } else {
                // flood arp reply to other ports
                for(int i = 0; i < sbuff->total_port_num; i++){
                    if(i != port_idx){
                        // modify mac addr
                        memcpy(eth->smac, sbuff->ports[i]->mac, 6);
                        nwrite = pkt_send(sbuff->ports[i], sbuff->pkt_buff, sbuff->nh + sizeof(arphdr));
                        if(nwrite > 0){
                            if(sbuff->debug) {
                                LOG_TO_SCREEN("(%d)[%04d] Flood ARP reply to port:%d", port_idx, sbuff->sent_pkts, i);
                                LOG_TO_SCREEN("(%d)[%04d] Send packet to port:%d (%d bytes)", port->idx, sbuff->sent_pkts, sbuff->ports[i]->idx, nwrite);
                            }
                            STATS_INC_SENT_PKT(sbuff);
                        }
                    }
                }
            }
            
            break;
        case ARP_OP_RREQ:
            // TODO:
            break;
        case ARP_OP_RRES:
            // TODO:
            break;
        default:
            // TODO: this case should not happend, abnormal packet
            LOG_TO_SCREEN("Error opcode: %d", arp_op);
            break;
    }
}
