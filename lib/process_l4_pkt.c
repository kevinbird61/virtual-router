#include "process_l4_pkt.h"

int
process_icmp_pkt(struct work_thrd_ctx_t *sbuff)
{
    u16 port_idx = sbuff->port_idx;
    port_t *port = sbuff->ports[port_idx];
    ethhdr *eth = (ethhdr *)(sbuff->pkt_buff);
    struct ipv4hdr *iph = (struct ipv4hdr *)(sbuff->pkt_buff + sbuff->nh);
    icmphdr *icmph = (icmphdr *)(sbuff->pkt_buff + sbuff->h);
    u16 nwrite = 0;

    switch(icmph->type){
        case ICMP_ECHO_REPLY:
            if(sbuff->debug){
                LOG_TO_SCREEN("---------------------------------------------------------");
                LOG_TO_SCREEN("(%d) ICMP type = %d, code = %d, %s", sbuff->port_idx, icmph->type, icmph->code, g_icmp_echo_str[icmph->type]);
            }
            break;
        case ICMP_ECHO_REQUEST:
            if(sbuff->debug){
                LOG_TO_SCREEN("---------------------------------------------------------");
                LOG_TO_SCREEN("(%d) ICMP type = %d, code = %d, %s", sbuff->port_idx, icmph->type, icmph->code, g_icmp_echo_str[icmph->type]);
            }

            /* send icmp echo reply back */
            iph->saddr = iph->saddr ^ iph->daddr;
            iph->daddr = iph->saddr ^ iph->daddr;
            iph->saddr = iph->saddr ^ iph->daddr;
            icmph->type = ICMP_ECHO_REPLY;
            icmph->checksum = 0; 
            icmph->checksum = calc_icmp_cksum((u16 *)icmph, ntohs(iph->tot_len) - sizeof(struct ipv4hdr));

            memcpy(eth->dmac, eth->smac, 6);
            memcpy(eth->smac, port->mac, 6);

            pkt_send(port, sbuff->pkt_buff, sbuff->h + ntohs(iph->tot_len));
            if(sbuff->debug){
                LOG_TO_SCREEN("(%d) Send ICMP %s.", port_idx, g_icmp_echo_str[icmph->type]);
                LOG_TO_SCREEN("(%d) Send packet to port:%d (%d bytes)", port->idx, port->idx, nwrite);
            }
            
            break;
        case ICMP_DEST_UNREACHABLE:
            break;
        default:
            // TODO
            break;
    }
}

int
process_tcp_pkt(struct work_thrd_ctx_t *sbuff)
{
    struct tcphdr *tcph = (struct tcphdr *)(sbuff->pkt_buff + sbuff->h);
    
    if(sbuff->debug){
        LOG_TO_SCREEN("---------------------------------------------------------");
        LOG_TO_SCREEN("(%d) TCP sport=%d, dport=%d (seqn=%d, ackn=%d, window=%d) [%c%c%c%c%c%c]", sbuff->port_idx, 
                ntohs(tcph->th_sport), ntohs(tcph->th_dport), ntohl(tcph->th_seq), ntohl(tcph->th_ack), ntohs(tcph->th_win),
                (tcph->th_flags & TH_FIN) > 0 ? 'F' : ' ', (tcph->th_flags & TH_SYN) > 0 ? 'S' : ' ', 
                (tcph->th_flags & TH_RST) > 0 ? 'R' : ' ', (tcph->th_flags & TH_PUSH) > 0 ? 'P' : ' ', 
                (tcph->th_flags & TH_ACK) > 0 ? 'A' : ' ', (tcph->th_flags & TH_URG) > 0 ? 'U' : ' ');
    }
}

int
process_udp_pkt(struct work_thrd_ctx_t *sbuff)
{
    struct udphdr *udph = (struct udphdr *)(sbuff->pkt_buff + sbuff->h);

    if(sbuff->debug){
        LOG_TO_SCREEN("---------------------------------------------------------");
        LOG_TO_SCREEN("(%d) UDP sport=%d, dport=%d", sbuff->port_idx, 
                ntohs(udph->uh_sport), ntohs(udph->uh_dport));
    }
}
