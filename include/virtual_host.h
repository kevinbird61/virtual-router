#ifndef __VIRTUAL_HOST__
#define __VIRTUAL_HOST__

/* maintain some virtual hosts owned by this virtual router. */

#include "types.h"

typedef struct virtual_host {
    u8  mac[6];
    u32 ip_addr;
} vhost;

#endif
