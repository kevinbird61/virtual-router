#ifndef __ETHER__
#define __ETHER__

enum EtherType
{
    ETH_IP = 0x0800,
    ETH_ARP = 0x0806,
    ETH_IP6 = 0x86DD
};

/* ethertype */
static const char *g_ethertype_str[] = {
    [ETH_IP]="ETH_IP",
    [ETH_ARP]="ETH_ARP",
    [ETH_IP6]="ETH_IPv6"
};

typedef struct ethhdr_t {
    unsigned char dmac[6];
    unsigned char smac[6];
    unsigned short ethertype;
} ethhdr;

#endif
