#ifndef __ARP__
#define __ARP__

#include "types.h"

#define ARP_TABLE_SIZE 32

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

/**
 * ARP entry, table:
 * 
 * | IP address | MAC address | interface   | ... |
 * | (32 bits)  | (48 bits)   | (port_t*)   | ... |
 */
enum ARP_Entry_State {
    AS_FREE = 0,        /* entry is unused */
    AS_INCOMPLETE = 1,  /* entry is used but incompleted */
    AS_RESOLVED = 2     /* entry has been resolved */
};

struct arp_entry {
    u8      mac[6];     // hardware address
    u32     ip;         // protocol address
    port_t  *port;   // pointer to interface/port
    u16     state;      // state of arp entry
    u16     hw_type;    // hardware type
    u16     proto_type; // protocol type
    u8      hw_len;     // hardware length
    u8      proto_len;  // protocol length
    // u32 queue_len;
    // u32 attempts;
    u32     age;
};

#endif
