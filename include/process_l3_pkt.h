#ifndef __L3_PKT__
#define __L3_PKT__

#include <arpa/inet.h>

#include "process_l4_pkt.h"
#include "router_ctx.h"
#include "logger.h"
#include "pkthdr.h"
#include "types.h"
#include "utils.h"

int process_arp_pkt(struct work_thrd_ctx_t *sbuff);
int process_ip_pkt(struct work_thrd_ctx_t *sbuff);

#endif
