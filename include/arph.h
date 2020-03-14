#ifndef __ARP__
#define __ARP__

#include "types.h"

enum ARP_OP 
{
    ARP_OP_REQ = 1,
    ARP_OP_RES = 2,
    ARP_OP_RREQ = 3,
    ARP_OP_RRES = 4
};

static const char *g_arp_op_str[] = {
    [ARP_OP_REQ] = "arp request", 
    [ARP_OP_RES] = "arp reply",
    [ARP_OP_RREQ] = "rarp request",
    [ARP_OP_RRES] = "rarp reply"
};

typedef struct _arphdr_t {
    u16     arp_hard_type;  // format of hw addr
    u16     arp_proto_type; // format of proto addr
    u8      arp_hard_size;  // length of hw address
    u8      arp_proto_size; // length of proto addr
    u16     arp_op;         // arp operation code (cmd)
    u8      arp_eth_src[6]; // smac
    u32     arp_ip_saddr;   // src ip
    u8      arp_eth_dst[6]; // dmac
    u32     arp_ip_daddr;   // dst ip
} __attribute__ ((packed)) arphdr;

#endif
