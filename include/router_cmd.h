#ifndef __ROUTER_CMD__
#define __ROUTER_CMD__

#include "router_ctx.h"

/* support command */
enum L1_CMD {
    CMD_DEBUG = 0,
    CMD_SHOW,
    CMD_HELP,
    CMD_LAST
};

static const char *L1_cmd[] = {
    [CMD_DEBUG] = "debug",
    [CMD_SHOW] = "show",
    [CMD_HELP] = "help",
    [CMD_LAST] = NULL
};

enum ROUTER_CMD {
    // debug 
    CMD_DEBUG_START = 0,
    CMD_DEBUG_PKT,
    CMD_DEBUG_NON,
    CMD_DEBUG_END,
    // show
    CMD_SHOW_START,
    CMD_SHOW_STATS,
    CMD_SHOW_PORTS,
    CMD_SHOW_END,
    // help 
    CMD_HELP_ALL,
    // end of router command
    CMD_ROUTE_CMD_END
};

/* description string */
static const char *desc_router_cmd[] = {
    // debug
    [CMD_DEBUG_PKT] = "Enable all debug message from all ports.",
    [CMD_DEBUG_NON] = "Disable all debug message from all ports.",
    // show
    [CMD_SHOW_STATS] = "Show all statistics collected by virtual router.",
    [CMD_SHOW_PORTS] = "Show all ports/interfaces information.",
    // help 
    [CMD_HELP_ALL] = "Print this help message."
};

static const char *L2_cmd[][32] = 
{
    /* debug command */
    [CMD_DEBUG_START] =     NULL,
    [CMD_DEBUG_PKT] =       {"packet", NULL},
    [CMD_DEBUG_NON] =       {"no", NULL},
    [CMD_DEBUG_END] =       NULL,
    /* show command */
    [CMD_SHOW_START] =      NULL,
    [CMD_SHOW_STATS] =      {"stats", NULL},
    [CMD_SHOW_PORTS] =      {"ports", NULL},
    [CMD_SHOW_END] =        NULL
};

/* hook func list */
static int (*cmd_hook_func[CMD_ROUTE_CMD_END]) (struct main_thrd_ctx_t *this);
/* hook func - debug */
int cmd_debug_packet(struct main_thrd_ctx_t *this);
int cmd_debug_disable(struct main_thrd_ctx_t *this);
/* hook func - show */
int cmd_show_stats(struct main_thrd_ctx_t *this);
int cmd_show_ports(struct main_thrd_ctx_t *this);
/* hook func - help */
int cmd_help_all(struct main_thrd_ctx_t *this);

#endif
