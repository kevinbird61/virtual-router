#include "router_ctx.h"
#include "router_cmd.h"
#include "types.h"

#include <string.h>
#include <stdio.h>

/**
 * main CLI loop
 */
int router_cli(struct main_thrd_ctx_t *this)
{
    char cmd[128], curr[32], rest[96];
    u8 L1_cmd_idx = 0, L2_idx = 0;

    cmd_hook_func[CMD_DEBUG_PKT] = debug_packet;

    printf("===============================================\n");
    while(1) {
        printf("myRouter> ");
        if(fgets(cmd, 128, stdin)){
            /* get first token to distinguish */
            sscanf(cmd, "%s %[^\n]", curr, rest);

            /* scan through L1 cmd list */ 
            for(L1_cmd_idx = 0; L1_cmd_idx < CMD_LAST; L1_cmd_idx++){
                if(strstr(L1_cmd[L1_cmd_idx], curr) != NULL) {
                    // printf("%s cmd\n", L1_cmd[L1_cmd_idx]);
                    switch(L1_cmd_idx){
                        /* ============================================= CMD_DEBUG ============================================= */
                        case CMD_DEBUG:
                            for(L2_idx = CMD_DEBUG_PKT; L2_idx < CMD_DEBUG_END; L2_idx++){
                                u8 iter_idx = 0, match_idx = 0;
                                while(debug_cmd[L2_idx][iter_idx] != NULL){
                                    // puts(debug_cmd[L2_idx][iter_idx]);
                                    sscanf(rest, "%s %[^\n]", curr, rest);
                                    if(strstr(debug_cmd[L2_idx][iter_idx], curr) != NULL){
                                        // match
                                        match_idx++;
                                    }
                                    iter_idx++; 
                                }

                                if( iter_idx == match_idx ) {
                                    // match total command, call its hook func 
                                    switch(L2_idx){
                                        case CMD_DEBUG_PKT:
                                            (*cmd_hook_func[CMD_DEBUG_PKT])(this);
                                            break;
                                        default: // not support
                                            puts("Invalid L2 idx");
                                            break; 
                                    }
                                } else { 
                                    puts("Wrong debug command");
                                }
                            }
                            break; 
                        /* ============================================= CMD_SHOW ============================================= */
                        case CMD_SHOW: 
                            break;
                        default: // show not happen
                            break;
                    }
                    break;
                }
            }
            if(L1_cmd_idx == CMD_LAST) {
                puts("Not supported command.");
            }
            L1_cmd_idx = 0;
        }
    }
}