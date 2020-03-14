#include "process_l3_pkt.h"

int 
process_ip_pkt(struct work_thrd_ctx_t *sbuff)
{
    struct ipv4hdr *iph = (struct ipv4hdr *) (sbuff->pkt_buff + sbuff->nh);
    char src_ip_str[INET_ADDRSTRLEN], dst_ip_str[INET_ADDRSTRLEN];
    sbuff->h = ntohs(iph->tot_len);
    
    if(sbuff->debug){
        inet_ntop(AF_INET, &iph->saddr, src_ip_str, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &iph->daddr, dst_ip_str, INET_ADDRSTRLEN);
        LOG_TO_SCREEN("---------------------------------------------------------");
        LOG_TO_SCREEN("(%d) saddr: %s, daddr: %s. IPPROTO: %s", sbuff->port_idx, src_ip_str, dst_ip_str, g_ip_proto_str[iph->proto]);
    }

    // process next layer
}

int
process_arp_pkt(struct work_thrd_ctx_t *sbuff)
{
    arphdr *arph = (arphdr *) (sbuff->pkt_buff + sbuff->nh);
    char arp_src_ip_str[INET_ADDRSTRLEN], arp_dst_ip_str[INET_ADDRSTRLEN];
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

            break;
        case ARP_OP_RES:
            if(sbuff->debug){
                LOG_TO_SCREEN("---------------------------------------------------------");
                LOG_TO_SCREEN("(%d) %s, %s is-at %x.%x.%x.%x.%x.%x", sbuff->port_idx, 
                        g_arp_op_str[arp_op], arp_dst_ip_str, 
                        arph->arp_eth_dst[0], arph->arp_eth_dst[1], arph->arp_eth_dst[2],
                        arph->arp_eth_dst[3], arph->arp_eth_dst[4], arph->arp_eth_dst[5]);
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
