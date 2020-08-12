#include "router_ctx.h"
#include "router_cmd.h"
#include "types.h"

#include <string.h>
#include <stdio.h>

#define CMD_BUF_SIZE        128
#define WORD_BUF_SIZE       32

/**
 * main CLI loop
 */
int router_cli(struct main_thrd_ctx_t *this)
{
    char cmd[CMD_BUF_SIZE], curr[WORD_BUF_SIZE], rest[CMD_BUF_SIZE-WORD_BUF_SIZE];
    u8 L1_cmd_idx = 0, L2_idx = 0, L2_idx_end = 255;

    // register hook function
    cmd_hook_func[CMD_DEBUG_PKT] =      cmd_debug_packet;
    cmd_hook_func[CMD_DEBUG_NON] =      cmd_debug_disable;
    cmd_hook_func[CMD_SHOW_STATS] =     cmd_show_stats;
    cmd_hook_func[CMD_SHOW_PORTS] =     cmd_show_ports;
    cmd_hook_func[CMD_HELP_ALL] =       cmd_help_all;

    LOG_TO_SCREEN("===============================================");
    while(1) {
        printf("myRouter> ");
        if(fgets(cmd, CMD_BUF_SIZE, stdin)){
            /* get first token to distinguish */
            sscanf(cmd, "%s %[^\n]", curr, rest);

            /* scan through L1 cmd list */ 
            for(L1_cmd_idx = 0; L1_cmd_idx < CMD_LAST; L1_cmd_idx++){
                if(strstr(L1_cmd[L1_cmd_idx], curr) != NULL) {
                    // printf("%s cmd\n", L1_cmd[L1_cmd_idx]);
                    
                    switch(L1_cmd_idx){
                        /* ============================================= CMD_DEBUG ============================================= */
                        case CMD_DEBUG:
                            L2_idx = CMD_DEBUG_START + 1;
                            L2_idx_end = CMD_DEBUG_END;
                            break; 
                        /* ============================================= CMD_SHOW ============================================= */
                        case CMD_SHOW: 
                            L2_idx = CMD_SHOW_START + 1;
                            L2_idx_end = CMD_SHOW_END;
                            break;
                        /* =============================================
 * CMD_HELP ============================================= */
                        case CMD_HELP:
                            L2_idx_end = 0;
                            (*cmd_hook_func[CMD_HELP_ALL])(this);
                            break;
                        default: // show not happen
                            break;
                    }

                    /* handle command here */
                    for(L2_idx; L2_idx < L2_idx_end; L2_idx++){
                        u8 iter_idx = 0, match_idx = 0;
                        sscanf(cmd, "%s %[^\n]", curr, rest); // refresh
                        while(L2_cmd[L2_idx][iter_idx] != NULL){
                            sscanf(rest, "%s %[^\n]", curr, rest);
                            if(strstr(L2_cmd[L2_idx][iter_idx], curr) != NULL){
                                match_idx++;
                            }
                            iter_idx++; 
                        }

                        if( iter_idx == match_idx && iter_idx != 0 && match_idx != 0 ) {
                            // match total command, call its hook func 
                            if( (L2_idx > CMD_DEBUG_START && L2_idx < CMD_DEBUG_END) ||
                                    (L2_idx > CMD_SHOW_START && L2_idx < CMD_SHOW_END)) {
                                // valid command 
                                (*cmd_hook_func[L2_idx])(this);
                                break;
                            } else {
                                LOG_TO_SCREEN("Invalid L2 idx");
                                break;
                            }
                        }
                    }

                    break;
                }
            }

            if(L1_cmd_idx == CMD_LAST) {
                LOG_TO_SCREEN("Not supported command.");
            }
            L1_cmd_idx = 0;

            memset(cmd, 0x00, CMD_BUF_SIZE);
            memset(curr, 0x00, WORD_BUF_SIZE);
            memset(rest, 0x00, CMD_BUF_SIZE - WORD_BUF_SIZE);
        }
    }
}
