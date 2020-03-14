#ifndef __L3_PKT__
#define __L3_PKT__

#include <arpa/inet.h>

#include "router_ctx.h"
#include "logger.h"
#include "types.h"
#include "utils.h"
#include "ether.h"
#include "arph.h"
#include "iph.h"

int process_arp_pkt(struct work_thrd_ctx_t *sbuff);
int process_ip_pkt(struct work_thrd_ctx_t *sbuff);

#endif
