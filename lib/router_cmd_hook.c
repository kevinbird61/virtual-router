#include "router_cmd.h"

#include <stdio.h>

/* ==================================== debug ==================================== */
int 
debug_packet(struct main_thrd_ctx_t *this)
{
    LOG_TO_SCREEN("Enable all ports' debug mode");

    u16 total_num_port = this->user_args->num_port;
    struct work_thrd_ctx_t **workers = this->workers;

    for(u16 i = 0; i < total_num_port; i++){
        workers[i]->debug = 1;
    }

    return 0;
}

int 
debug_disable(struct main_thrd_ctx_t *this)
{
    LOG_TO_SCREEN("Disable all ports' debug mode");

    u16 total_num_port = this->user_args->num_port;
    struct work_thrd_ctx_t **workers = this->workers;

    for(u16 i = 0; i < total_num_port; i++){
        workers[i]->debug = 0;
    }

    return 0;
}

/* ==================================== show ==================================== */


/* ==================================== help ==================================== */
int 
help_all(struct main_thrd_ctx_t *this)
{
    u8  L1_cmd_idx = 0, L2_cmd_idx = 0;

    LOG_TO_SCREEN("List all support CLI command:");
    for(L1_cmd_idx = CMD_DEBUG; L1_cmd_idx < CMD_LAST; L1_cmd_idx++){
        LOG_TO_SCREEN("* %s: ", L1_cmd[L1_cmd_idx]);
        switch(L1_cmd_idx){
            case CMD_DEBUG:
                for(L2_cmd_idx = CMD_DEBUG_START+1; L2_cmd_idx < CMD_DEBUG_END; L2_cmd_idx++){
                    u8 iter_idx = 0;
                    printf("\t%s ", L1_cmd[L1_cmd_idx]);
                    while(debug_cmd[L2_cmd_idx][iter_idx] != NULL){
                        printf("%s ", debug_cmd[L2_cmd_idx][iter_idx]);
                        iter_idx++;
                    }
                    // print description here
                    printf(": %s\n", desc_router_cmd[L2_cmd_idx]);
                }
                break;
            case CMD_SHOW:
                break;
            case CMD_HELP:
                printf("\t%s ", L1_cmd[CMD_HELP]);
                printf(": %s\n", desc_router_cmd[CMD_HELP_ALL]);
                break;
        }
    }
}
