#ifndef __ROUTER_CTX__
#define __ROUTER_CTX__

#include <pthread.h>

#include "argparse.h"
#include "types.h"
#include "port.h"

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
 * | IP address | MAC address | interface   |
 * | (32 bits)  | (48 bits)   | (obj_ptr)   |
 */



/** 
 * MAC table
 * 
 */


/** 
 * Routing table
 * 
 */

#endif
