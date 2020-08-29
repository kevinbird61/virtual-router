#ifndef __DEBUGGER__
#define __DEBUGGER__

#include "process_l2_pkt.h"
#include "process_l3_pkt.h"
#include "process_l4_pkt.h"
#include "logger.h"

enum Packet_log_type{
    LOG_ETH = 1,
    LOG_IP,
    LOG_IP6,
    LOG_ARP,
    LOG_TCP,
    LOG_UDP,
    LOG_ICMP,
    LOG_INVALID_TYPE
};

#define LOG_PKT_START()                 LOG_TO_SCREEN("%s", "-----------------------------------------------")
#define LOG_PKT_END()                   LOG_TO_SCREEN("%s", "-----------------------------------------------")
#define LOG_PKT_INFO(sbuff, pkt_log_t)  packet_log(sbuff, pkt_log_t)

static inline int
packet_log(struct work_thrd_ctx_t *sbuff, u16 pkt_log_type)
{
    if (sbuff->debug) {
        switch (pkt_log_type) {
            case LOG_ETH:{
                ethhdr *eth = (ethhdr *) (sbuff->pkt_buff);
                LOG_TO_SCREEN("(%d)[%04d] dmac: %x.%x.%x.%x.%x.%x, smac: %x.%x.%x.%x.%x.%x, ethertype: %s", 
                    sbuff->port_idx, sbuff->sent_pkts,
                    eth->dmac[0], eth->dmac[1], eth->dmac[2], eth->dmac[3], eth->dmac[4], eth->dmac[5],
                    eth->smac[0], eth->smac[1], eth->smac[2], eth->smac[3], eth->smac[4], eth->smac[5],
                    g_ethertype_str[ntohs(eth->ethertype)]);
            }
            break;
            case LOG_IP:{
                struct ipv4hdr *iph = (struct ipv4hdr *) (sbuff->pkt_buff + sbuff->nh);
                char src_ip_str[INET_ADDRSTRLEN], dst_ip_str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &iph->saddr, src_ip_str, INET_ADDRSTRLEN);
                inet_ntop(AF_INET, &iph->daddr, dst_ip_str, INET_ADDRSTRLEN);
                LOG_TO_SCREEN("(%d)[%04d] saddr: %s, daddr: %s. proto: %s",
                    sbuff->port_idx, sbuff->sent_pkts,
                    src_ip_str, dst_ip_str, g_ip_proto_str[iph->proto]);
            }
            break;
            case LOG_ARP: {
                arphdr *arph = (arphdr *) (sbuff->pkt_buff + sbuff->nh);
                char arp_src_ip_str[INET_ADDRSTRLEN], arp_dst_ip_str[INET_ADDRSTRLEN];
                u16 arp_op = ntohs(arph->arp_op);
                inet_ntop(AF_INET, &arph->arp_ip_saddr, arp_src_ip_str, INET_ADDRSTRLEN);
                inet_ntop(AF_INET, &arph->arp_ip_daddr, arp_dst_ip_str, INET_ADDRSTRLEN);
                
                switch (arp_op) {
                    case ARP_OP_REQ:
                        LOG_TO_SCREEN("(%d)[%04d] %s, who has %s tell %s (%x.%x.%x.%x.%x.%x)", 
                            sbuff->port_idx, sbuff->sent_pkts,
                            g_arp_op_str[arp_op], arp_dst_ip_str, arp_src_ip_str, 
                            arph->arp_eth_src[0], arph->arp_eth_src[1], arph->arp_eth_src[2],
                            arph->arp_eth_src[3], arph->arp_eth_src[4], arph->arp_eth_src[5]);
                    break;
                    case ARP_OP_RES: 
                        LOG_TO_SCREEN("(%d)[%04d] %s, %s is-at %x.%x.%x.%x.%x.%x", 
                            sbuff->port_idx, sbuff->sent_pkts, 
                            g_arp_op_str[arp_op], arp_src_ip_str, 
                            arph->arp_eth_src[0], arph->arp_eth_src[1], arph->arp_eth_src[2],
                            arph->arp_eth_src[3], arph->arp_eth_src[4], arph->arp_eth_src[5]);
                    break;
                    case ARP_OP_RREQ: // TODO
                    case ARP_OP_RRES: // TODO
                    default:
                    break;
                }
            }
            break;
            case LOG_TCP: {
                struct tcphdr *tcph = (struct tcphdr *)(sbuff->pkt_buff + sbuff->h);

                LOG_TO_SCREEN("(%d)[%04d] TCP sport=%d, dport=%d (seqn=%d, ackn=%d, window=%d) [%c%c%c%c%c%c]", 
                    sbuff->port_idx, sbuff->recv_pkts,
                    ntohs(tcph->th_sport), ntohs(tcph->th_dport), ntohl(tcph->th_seq), ntohl(tcph->th_ack), ntohs(tcph->th_win),
                    (tcph->th_flags & TH_FIN) > 0 ? 'F' : ' ', (tcph->th_flags & TH_SYN) > 0 ? 'S' : ' ', 
                    (tcph->th_flags & TH_RST) > 0 ? 'R' : ' ', (tcph->th_flags & TH_PUSH) > 0 ? 'P' : ' ', 
                    (tcph->th_flags & TH_ACK) > 0 ? 'A' : ' ', (tcph->th_flags & TH_URG) > 0 ? 'U' : ' ');
            }
            break;
            case LOG_UDP: {
                struct udphdr *udph = (struct udphdr *)(sbuff->pkt_buff + sbuff->h);

                LOG_TO_SCREEN("(%d)[%04d] UDP sport=%d, dport=%d", 
                    sbuff->port_idx, sbuff->recv_pkts,
                    ntohs(udph->uh_sport), ntohs(udph->uh_dport));
            }
            break;
            case LOG_ICMP:{
                icmphdr *icmph = (icmphdr *)(sbuff->pkt_buff + sbuff->h);
                switch (icmph->type) {
                    case ICMP_ECHO_REPLY: 
                        LOG_TO_SCREEN("(%d)[%04d] ICMP type = %d, code = %d, %s", 
                            sbuff->port_idx, sbuff->recv_pkts,
                            icmph->type, icmph->code, g_icmp_echo_str[icmph->type]);
                    break;
                    case ICMP_ECHO_REQUEST:
                        LOG_TO_SCREEN("(%d)[%04d] ICMP type = %d, code = %d, %s", 
                            sbuff->port_idx, sbuff->recv_pkts,
                            icmph->type, icmph->code, g_icmp_echo_str[icmph->type]);
                    break;
                    case ICMP_DEST_UNREACHABLE: // TODO
                    default:
                    break;
                }
            }
            break;
            default:
                // not support yet
            break;
        }
    } // sbuff->debug

    return 0;
}

#endif
