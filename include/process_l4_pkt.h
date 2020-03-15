#ifndef __L4_PKT__
#define __L4_PKT__

#include "process_l2_pkt.h"
#include "process_l3_pkt.h"
#include "router_ctx.h"
#include "checksum.h"
#include "logger.h"
#include "types.h"
#include "utils.h"
#include "icmph.h"
#include "tcph.h"
#include "udph.h"

int process_icmp_pkt(struct work_thrd_ctx_t *sbuff);
int process_tcp_pkt(struct work_thrd_ctx_t *sbuff);
int process_udp_pkt(struct work_thrd_ctx_t *sbuff);

#endif
