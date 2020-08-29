#include "process_l4_pkt.h"
#include "debugger.h"

int
process_icmp_pkt(struct work_thrd_ctx_t *sbuff)
{
    u16 port_idx = sbuff->port_idx;
    port_t *port = sbuff->ports[port_idx];
    ethhdr *eth = (ethhdr *)(sbuff->pkt_buff);
    struct ipv4hdr *iph = (struct ipv4hdr *)(sbuff->pkt_buff + sbuff->nh);
    icmphdr *icmph = (icmphdr *)(sbuff->pkt_buff + sbuff->h);
    u16 nwrite = 0;

    LOG_PKT_INFO(sbuff, LOG_ICMP);

    switch(icmph->type){
        case ICMP_ECHO_REPLY:
            break;
        case ICMP_ECHO_REQUEST:
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
                LOG_TO_SCREEN("(%d)[%04d] Send ICMP %s.", port_idx, sbuff->recv_pkts, g_icmp_echo_str[icmph->type]);
                LOG_TO_SCREEN("(%d)[%04d] Send packet to port:%d (%d bytes)", port->idx, sbuff->recv_pkts, port->idx, nwrite);
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
    
    LOG_PKT_INFO(sbuff, LOG_TCP);    
}

int
process_udp_pkt(struct work_thrd_ctx_t *sbuff)
{
    struct udphdr *udph = (struct udphdr *)(sbuff->pkt_buff + sbuff->h);

    LOG_PKT_INFO(sbuff, LOG_UDP);
}
