#include "process_l3_pkt.h"

int 
process_ip_pkt(struct work_thrd_ctx_t *sbuff)
{
    struct ipv4hdr *iph = (struct ipv4hdr *) (sbuff->pkt_buff + sbuff->nh);
    char src_ip_str[INET_ADDRSTRLEN], dst_ip_str[INET_ADDRSTRLEN];
    sbuff->h = sbuff->nh + iph->ihl*4;
    
    if(sbuff->debug){
        inet_ntop(AF_INET, &iph->saddr, src_ip_str, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &iph->daddr, dst_ip_str, INET_ADDRSTRLEN);
        LOG_TO_SCREEN("---------------------------------------------------------");
        LOG_TO_SCREEN("(%d) saddr: %s, daddr: %s. IPPROTO: %s", sbuff->port_idx, src_ip_str, dst_ip_str, g_ip_proto_str[iph->proto]);
    }

    // FIXME: IP options

    // process next layer
    // FIXME: need to check if this packet is for us, or supported network function
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
    ethhdr *eth = (ethhdr *) (sbuff->pkt_buff);
    arphdr *arph = (arphdr *) (sbuff->pkt_buff + sbuff->nh);
    char arp_src_ip_str[INET_ADDRSTRLEN], arp_dst_ip_str[INET_ADDRSTRLEN];
    u8  found = 0, entry_idx = 0;
    u16 arp_op = ntohs(arph->arp_op);
    u16 nwrite = 0;

    inet_ntop(AF_INET, &arph->arp_ip_saddr, arp_src_ip_str, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &arph->arp_ip_daddr, arp_dst_ip_str, INET_ADDRSTRLEN);

    switch(arp_op){
        case ARP_OP_REQ:
            if(sbuff->debug){
                LOG_TO_SCREEN("---------------------------------------------------------");
                LOG_TO_SCREEN("(%d) %s, who has %s tell %s (%x.%x.%x.%x.%x.%x)", sbuff->port_idx, 
                        g_arp_op_str[arp_op], arp_dst_ip_str, arp_src_ip_str, 
                        arph->arp_eth_src[0], arph->arp_eth_src[1], arph->arp_eth_src[2],
                        arph->arp_eth_src[3], arph->arp_eth_src[4], arph->arp_eth_src[5]);
            }
            
            // DEBUG: send arp reply back (need to check if this ip is our ip address)
            /*
            ethhdr *eth = (ethhdr *) sbuff->pkt_buff;
            memcpy(eth->dmac, eth->smac, 6);
            memcpy(eth->smac, sbuff->ports[sbuff->port_idx]->mac, 6);
            
            arph->arp_ip_saddr = arph->arp_ip_saddr ^ arph->arp_ip_daddr;
            arph->arp_ip_daddr = arph->arp_ip_saddr ^ arph->arp_ip_daddr;
            arph->arp_ip_saddr = arph->arp_ip_saddr ^ arph->arp_ip_daddr;
            memcpy(arph->arp_eth_dst, arph->arp_eth_src, 6);
            memcpy(arph->arp_eth_src, arph->arp_eth_dst, 6);

            arph->arp_eth_src[1] = 0x26; // change to other value (pretend to be another mac addr, like concept of virtual server) 
            arph->arp_op = htons(ARP_OP_RES);

            // send packet out, this function need to be placed in another file
            nwrite = write(sbuff->ports[sbuff->port_idx]->fd, sbuff->pkt_buff, sbuff->nh + sizeof(arphdr));
            LOG_TO_SCREEN("(%d) Send back ARP reply for %s (%d bytes)", sbuff->port_idx, arp_dst_ip_str, nwrite);
            */

            // lookup arp table:
            // - if not found, then broadcast to other ports
            // - after lookup, we can update arp table of this source.
            pthread_spin_trylock(&arp_tb_lock);
            for(u8 i = 0; i < ARP_TABLE_SIZE; i++){
                switch(arp_tb[i].state){
                    case AS_FREE:
                        // in lookup process, this state is useless.
                        break;
                    case AS_INCOMPLETE:
                    case AS_RESOLVED:
                        // check the mac and ip is fit or not
                        if((memcmp(eth->dmac, arp_tb[i].mac, 6) == 0) && (arph->arp_ip_daddr == arp_tb[i].ip)){
                            // found the target 
                            found = 1;
                            entry_idx = i;
                        }
                        break;
                    default:
                        // should not happend.
                        break;
                }
            }

            // update arp table
            for(u8 i = 0; i < ARP_TABLE_SIZE; i++){
                if(arp_tb[i].state == AS_FREE){
                    // set state to AS_INCOMPLETE
                    arp_tb[i].state = AS_INCOMPLETE;
                    
                    // fill arp packet
                    // FIXME: other arp entry's info
                    memcpy(arp_tb[i].mac, eth->dmac, 6);
                    arp_tb[i].ip = arph->arp_ip_daddr;
                    arp_tb[i].port = sbuff->ports[sbuff->port_idx];
                    
                    break;
                }
            }

            // after lookup and update, release the lock
            pthread_spin_unlock(&arp_tb_lock);

            // if found == 0, then we need to flood this arp to other ports
            if(!found){
                for(int i = 0; i < sbuff->total_port_num; i++){
                    if(i != sbuff->port_idx){
                        // modify mac addr
                        memcpy(eth->smac, sbuff->ports[i]->mac, 6);
                        nwrite = write(sbuff->ports[i]->fd, sbuff->pkt_buff, sbuff->nh + sizeof(arphdr));
                        LOG_TO_SCREEN("(Forward to %d) Flood ARP request (%d bytes)", i, nwrite);
                    }
                }
            } else {
                // base on the entry info to forward the packet 
                memcpy(eth->smac, sbuff->ports[sbuff->port_idx]->mac, 6);
                nwrite = write(arp_tb[entry_idx].port->fd, sbuff->pkt_buff, sbuff->nh + sizeof(arphdr));
                LOG_TO_SCREEN("(Send to %s) Send ARP request by arp cache (%d bytes)", arp_tb[entry_idx].port->dev_name, nwrite);
            }

            break;
        case ARP_OP_RES:
            if(sbuff->debug){
                LOG_TO_SCREEN("---------------------------------------------------------");
                LOG_TO_SCREEN("(%d) %s, %s is-at %x.%x.%x.%x.%x.%x", sbuff->port_idx, 
                        g_arp_op_str[arp_op], arp_dst_ip_str, 
                        arph->arp_eth_dst[0], arph->arp_eth_dst[1], arph->arp_eth_dst[2],
                        arph->arp_eth_dst[3], arph->arp_eth_dst[4], arph->arp_eth_dst[5]);
            }

            // update arp table
            pthread_spin_trylock(&arp_tb_lock);
            for(u8 i = 0; i < ARP_TABLE_SIZE; i++){
                if(arp_tb[i].state == AS_INCOMPLETE){
                    if((memcmp(arp_tb[i].mac, eth->smac, 6) == 0) && arp_tb[i].ip == arph->arp_ip_saddr){
                        // match
                        arp_tb[i].state == AS_RESOLVED;
                    }
                }
            }
            pthread_spin_unlock(&arp_tb_lock);

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
