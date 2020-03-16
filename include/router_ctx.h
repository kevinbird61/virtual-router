#ifndef __ROUTER_CTX__
#define __ROUTER_CTX__

#include <pthread.h>

#include "argparse.h"
#include "types.h"
#include "port.h"

#define ARP_TABLE_SIZE 32

/** main thread */
struct main_thrd_ctx_t {
    argparse_t              *user_args;
};

/** worker thread */
struct work_thrd_ctx_t {
    /* thrd info */
    pthread_t               tid;
    /* port/intf info */
    port_t                  **ports;
    u16                     total_port_num;
    u16                     port_idx;
    /* ctrl */
    u8                      debug: 1,
                            spare: 7;
    /* pkt info */
    u8                      *pkt_buff;
    u8                      nh; // (struct iphdr *)(pkt_buff + nh)
    u8                      h;  // (struct tcphdr *)(pkt_buff + h)
};

/* ======================== worker_func.c ======================== */
int run_port(struct work_thrd_ctx_t *this);

/* ======================== routing info ======================== */
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

extern pthread_spinlock_t arp_tb_lock;
extern struct arp_entry arp_tb[ARP_TABLE_SIZE];

/** 
 * Routing table (v4)
 * 
 * | IP address | Netmask   | Gateway   | Interface |
 * | (32 bits)  | (32 bits) | (32 bits) | (port_t*) |
 */

#endif
