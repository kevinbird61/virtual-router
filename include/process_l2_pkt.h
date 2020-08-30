#ifndef __L2_PKT__
#define __L2_PKT__

#include "process_l3_pkt.h"
#include "router_ctx.h"
#include "logger.h"
#include "pkthdr.h"
#include "types.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>

int process_eth_pkt(struct work_thrd_ctx_t *sbuff);

#endif
