#include "process_l4_pkt.h"

int
process_icmp_pkt(struct work_thrd_ctx_t *sbuff)
{
    icmphdr *icmph = (icmphdr *)(sbuff->pkt_buff + sbuff->h);
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

            // DEBUG: send back icmp echo reply
            /*
            struct ipv4hdr *iph = (struct ipv4hdr *)(sbuff->pkt_buff + sbuff->nh);
            iph->saddr = iph->saddr ^ iph->daddr;
            iph->daddr = iph->saddr ^ iph->daddr;
            iph->saddr = iph->saddr ^ iph->daddr;

            icmph->type = ICMP_ECHO_REPLY;
            icmph->checksum = 0; 
            icmph->checksum = calc_icmp_cksum((u16 *)icmph, 64);

            ethhdr *eth = (ethhdr *)(sbuff->pkt_buff);
            memcpy(eth->dmac, eth->smac, 6);
            // memcpy(eth->smac, sbuff->ports[sbuff->port_idx]->mac, 6);
            eth->smac[1] = 0x26; // mac of our virtual host

            // send reply
            int nwrite = write(sbuff->ports[sbuff->port_idx]->fd, sbuff->pkt_buff, sbuff->h + 64);
            LOG_TO_SCREEN("(%d) Send back ICMP echo reply (%d bytes)", sbuff->port_idx, nwrite);
            */

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
