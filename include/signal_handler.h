#ifndef __SIG_HANDLER__
#define __SIG_HANDLER__

#include <stdio.h>
#include <signal.h>
#include "router_ctx.h"

static struct main_thrd_ctx_t *main_ptr;

static inline void
sig_handler(int signal)
{
    switch (signal) {
        case SIGINT: { /* using CTRL+C  disable debug mode */
            u16 total_num_port = main_ptr->user_args->num_port;
            struct work_thrd_ctx_t **workers = main_ptr->workers;
            for(u16 i = 0; i < total_num_port; i++){
                workers[i]->debug = 0;
            } 
            if (write(STDIN_FILENO, "\nmyRouter>", sizeof("\nmyRouter>")) < 0) {
                perror("Write STDIN_FILENO error");
            }
        }
        break;
        default:
        break;
    }
}

#define SIG_HANDLE(sig) (signal(sig, sig_handler))
#define SIG_INIT(thrd)  (main_ptr = thrd)

#endif
