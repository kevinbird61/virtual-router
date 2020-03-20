#ifndef __ROUTER_CMD_HOOK__
#define __ROUTER_CMD_HOOK__

#include "router_ctx.h"

#include <stdio.h>

/* hook for debug */
static int debug_packet(struct main_thrd_ctx_t *this)
{
    puts("Enable all ports' debug mode");

    u16 total_num_port = this->user_args->num_port;
    struct work_thrd_ctx_t **workers = this->workers;

    for(u16 i = 0; i < total_num_port; i++){
        workers[i]->debug = 1;
    }

    return 0;
}

#endif 