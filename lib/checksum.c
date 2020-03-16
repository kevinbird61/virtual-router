#include "checksum.h"

u16 
calc_icmp_cksum(u16 *icmph, int len)
{
    u16 ret = 0;
    u32 sum = 0xffff;

    while(len > 1){
        sum += *icmph++;
        len -= 2;
    }

    if(len == 1){
        sum += *(u8*)icmph;
    }

    sum = (sum & 0xffff) + (sum >> 16);
    sum += (sum >> 16);
    
    return ~sum;
}

u16
calc_tcp_cksum(u8 *pkt_buff)
{
    struct ipv4hdr *iph = (struct ipv4hdr *)(pkt_buff);
    struct tcphdr *tcph = (struct tcphdr *)(pkt_buff + (iph->ihl << 2));
    u8 *payload = pkt_buff + (iph->ihl << 2);

    register u64 sum = 0;
    u8 tcplen = (ntohs(iph->tot_len) - (iph->ihl << 2));
    
    // src ip
    sum += (iph->saddr >> 16) & 0xFFFF;
    sum += (iph->saddr) & 0xFFFF;
    // dst ip
    sum += (iph->daddr >> 16) & 0xFFFF;
    sum += (iph->daddr) & 0xFFFF;
    // proto
    sum += htons(IPPROTO_TCP);
    // payload (tcp)
    tcph->check = 0;
    while(tcplen > 1){
        sum += *((payload)++);
        tcplen -= 2;
    }

    if(tcplen > 0){
        sum += (*(payload) & htons(0xFF00));
    }

    while(sum >> 16){
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    sum = ~sum;

    return sum;
}

u16
calc_udp_cksum(u8 *pkt_buff)
{
    struct ipv4hdr *iph = (struct ipv4hdr *)(pkt_buff);
    struct udphdr *udph = (struct udphdr *)(pkt_buff + (iph->ihl << 2));
    u8 *payload;
    payload = pkt_buff + (iph->ihl << 2);

    register u64 sum = 0;
    u16 udplen = htons(udph->len);
    
    // src ip
    sum += (iph->saddr >> 16) & 0xFFFF;
    sum += (iph->saddr) & 0xFFFF;
    // dst ip
    sum += (iph->daddr >> 16) & 0xFFFF;
    sum += (iph->daddr) & 0xFFFF;
    // proto
    sum += htons(IPPROTO_UDP);
    // length 
    sum += udph->len;

    udph->check = 0;
    while(udplen > 1){
        sum += *(payload++);
        udplen -= 2;
    }

    if(udplen > 0){
        sum += ((*payload) & htons(0xFF00));
    }

    while(sum >> 16){
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    sum = ~sum;
    sum = ((u8)sum == 0x0000) ? 0xFFFF : (u16) sum;

    return sum;
}
