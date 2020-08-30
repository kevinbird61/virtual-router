#ifndef __ICMP__
#define __ICMP__

#include "types.h"

enum ICMP_Type {
    ICMP_ECHO_REPLY = 0,
    ICMP_DEST_UNREACHABLE = 3,
    ICMP_REDIRECT_MSG = 5,
    ICMP_ECHO_REQUEST = 8,
    ICMP_ROUTER_ADV = 9,
    ICMP_ROUTER_SOL = 10,
    ICMP_TIME_EXCEED = 11
};

static const char *g_icmp_echo_str[] = {
    [ICMP_ECHO_REPLY] = "Echo reply",
    [ICMP_ECHO_REQUEST] = "Echo request"
};

static const char *g_icmp_dest_unreachable_str[] = {
    [ICMP_DEST_UNREACHABLE] = "Destination network unreachable",
    [ICMP_DEST_UNREACHABLE + 1] = "Destination host unreachable",
    [ICMP_DEST_UNREACHABLE + 2] = "Destination protocol unreachable",
    [ICMP_DEST_UNREACHABLE + 3] = "Destination port unreachable",
    [ICMP_DEST_UNREACHABLE + 4] = "Fragmentation required, and DF flag set",
    [ICMP_DEST_UNREACHABLE + 5] = "Source route failed",
    [ICMP_DEST_UNREACHABLE + 6] = "Destination network unknown",
    [ICMP_DEST_UNREACHABLE + 7] = "Destination host unknown",
    [ICMP_DEST_UNREACHABLE + 8] = "Source host isolated",
    [ICMP_DEST_UNREACHABLE + 9] = "Network administratively prohibited",
    [ICMP_DEST_UNREACHABLE + 10] = "Host administratively prohibited",
    [ICMP_DEST_UNREACHABLE + 11] = "Network unreachable for ToS",
    [ICMP_DEST_UNREACHABLE + 12] = "Host unreachable for ToS",
    [ICMP_DEST_UNREACHABLE + 13] = "Communication administratively prohibited",
    [ICMP_DEST_UNREACHABLE + 14] = "Host Precedence Violation",
    [ICMP_DEST_UNREACHABLE + 15] = "Precedence cutoff in effect"
};

/* other str: TODO */

typedef struct _icmphdr_t {
    u8  type;
    u8  code;
    u16 checksum;
    union {
        struct {
            u16 id;
            u16 seq;
        } echo;
        u32 gateway;
        struct {
            u16 __unused;
            u16 mtu;
        } frag;
    } un;
} icmphdr;

#endif
