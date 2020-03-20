#ifndef __ROUTER_CTX__
#define __ROUTER_CTX__

#include <pthread.h>

#include "argparse.h"
#include "logger.h"
#include "types.h"
#include "port.h"
#include "arph.h"

/* ======================== routing info ======================== */
/** 
 * Routing table (v4)
 * 
 * | IP address | Netmask   | Gateway   | Interface |
 * | (32 bits)  | (32 bits) | (32 bits) | (port_t*) |
 */
#define ROUTE_TABLE_SIZE 32

struct route_entry {
    u8                      is_used: 1,
                            spare: 7;
    u32                     ip;
    u32                     netmask;
    u32                     gw;
    port_t                  *port;
};

extern struct route_entry       route_table[ROUTE_TABLE_SIZE];
extern pthread_spinlock_t       arp_lock;
extern struct arp_entry         arp_tb[ARP_TABLE_SIZE];

/** main thread */
struct main_thrd_ctx_t {
    argparse_t              *user_args;
    struct work_thrd_ctx_t  **workers;
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
    /* other */
};

/* ======================== worker_func.c ======================== */
int run_port(struct work_thrd_ctx_t *this);

/* ======================== main_func.c ======================== */
int router_cli(struct main_thrd_ctx_t *this);

/* ======================== arp.c ======================== */
// arp reply
int resolve_arp_cache(struct work_thrd_ctx_t *sbuff);
// arp request
int add_new_arp_cache(struct work_thrd_ctx_t *sbuff);
// fill dmac with arp cache
int fill_dmac_by_arp(u32 ip, u8 *mac);

/* ======================== route.c ======================== */
int add_route(u32 ip, u32 netmask, u32 gateway, port_t *port);
port_t *lookup_route(u32 target_ip);

/* ======================== pkt_send.c ======================== */
int pkt_send(port_t *port, u8 *pkt_buff, u16 len);

#endif
