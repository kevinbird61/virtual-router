#include "router_ctx.h"
#include "argparse.h"
#include "logger.h"

#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/** software router
 * - main thread:
 *      - provide CLI to operate
 *      - using non-blocking IO control all ports (e.g. tap clients)
 *      - each worker thread own one port (interface)
 *      - poll the packet from those interface, read packet information
 *          , if this packet isn't for this router, then forward it.
 */

// arp table 
pthread_spinlock_t arp_tb_lock;
struct arp_entry arp_tb[ARP_TABLE_SIZE];

int main(int argc, char *argv[])
{
    struct main_thrd_ctx_t *main_thrd = NULL;
    struct work_thrd_ctx_t **work_thrds = NULL;

    // alloc
    SAVE_ALLOC(main_thrd, 1, struct main_thrd_ctx_t *);
    SAVE_ALLOC(main_thrd->user_args, 1, argparse_t *);
    parse_args(argc, argv, main_thrd->user_args);
    SAVE_ALLOC(work_thrds, main_thrd->user_args->num_port, struct work_thrd_ctx_t **);
    if(pthread_spin_init(&arp_tb_lock, PTHREAD_PROCESS_SHARED) != 0){
        perror("pthread_spin_init fail: ");
        exit(1);
    }

    // print user args
    printf("Number of ports: %d\n", main_thrd->user_args->num_port);
    for(int i = 0; i < main_thrd->user_args->num_port; i++){
        LOG_TO_SCREEN(" - %s", main_thrd->user_args->port[i]->dev_name);
        // init port
        main_thrd->user_args->port[i]->fd = create_port(main_thrd->user_args->port[i]->dev_name, IFF_TAP | IFF_NO_PI);
        get_dev_info(main_thrd->user_args->port[i]);

        if(main_thrd->user_args->port[i]->fd < 0){
            perror("fail to connect to tun/tap");
            exit(1);
        }
        LOG_TO_SCREEN("Connect to '%s' successfully.", main_thrd->user_args->port[i]->dev_name);
    }
    
    // simulate port(s)
    for (int i = 0; i < main_thrd->user_args->num_port; i++){
        // init work_thrd info
        SAVE_ALLOC(work_thrds[i], 1, struct work_thrd_ctx_t *);
        work_thrds[i]->ports = main_thrd->user_args->port;
        work_thrds[i]->total_port_num = main_thrd->user_args->num_port;
        work_thrds[i]->port_idx = i;

        if(pthread_create(&work_thrds[i]->tid, NULL, (void *)&run_port, work_thrds[i]) < 0){
            perror("pthread_create failed.");
            break;
        }
        LOG_TO_SCREEN("Port(%d) create successfully, tid = %u.", i, work_thrds[i]->tid);
    }

    for(int i = 0; i < main_thrd->user_args->num_port; i++){
        pthread_join(work_thrds[i]->tid, NULL);
    }

    // main thread's CLI
    //while(1){
        // TODO: put main CLI loop into another func
    //}

    return 0;
}
