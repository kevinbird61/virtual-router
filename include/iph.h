#ifndef __IP__
#define __IP__

#include <linux/ip.h>

#include "types.h"

// protocol number: netinet/in.h

static const char *g_ip_proto_str[] = {
    [IPPROTO_ICMP] = "ICMP",
    [IPPROTO_TCP] = "TCP",
    [IPPROTO_UDP] = "UDP"
};

// ipv4 header
struct ipv4hdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    u8  ihl: 4,
        version: 4;
#elif defined(__BIG_ENDIAN_BITFIELD)
    u8  version: 4,
        ihl: 4;
#else 
    #error "Please fix <asm/byteorder.h>"
#endif
    u8  tos;
    u16 tot_len;
    u16 id;
    u16 frag_off;
    u8  ttl;
    u8  proto;
    u16 check;
    u32 saddr;
    u32 daddr;
    /* the options start here */
};

#endif
