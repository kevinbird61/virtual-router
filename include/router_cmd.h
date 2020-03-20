#ifndef __ROUTER_CMD__
#define __ROUTER_CMD__

#include "router_cmd_hook.h"

/* support command */
enum L1_CMD {
    CMD_DEBUG = 0,
    CMD_SHOW,
    CMD_LAST
};

static const char *L1_cmd[] = {
    [CMD_DEBUG] = "debug",
    [CMD_SHOW] = "show",
    [CMD_LAST] = NULL
};

enum ROUTER_CMD {
    // debug 
    CMD_DEBUG_PKT = 0,
    CMD_DEBUG_END,
    // show
    CMD_SHOW_STATS,
    // end of router command
    CMD_ROUTE_CMD_END
};

/* debug command */
static const char *debug_cmd[][32] = 
{
    [CMD_DEBUG_PKT] = {"packet", NULL},
    [CMD_DEBUG_END] = NULL
};

static int (*cmd_hook_func[CMD_ROUTE_CMD_END]) (struct main_thrd_ctx_t *this);


#endif