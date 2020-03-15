#ifndef __CHECKSUM__
#define __CHECKSUM__

#include "process_l4_pkt.h"
#include "router_ctx.h"
#include "types.h"

u16 calc_icmp_cksum(u16 *icmph, int len);

#endif
