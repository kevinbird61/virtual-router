#include "router_cmd.h"

#include <stdio.h>

/* ==================================== debug ==================================== */
int 
cmd_debug_packet(struct main_thrd_ctx_t *this)
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
cmd_debug_disable(struct main_thrd_ctx_t *this)
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
int 
cmd_show_stats(struct main_thrd_ctx_t *this)
{
    LOG_TO_SCREEN("Show all router's statistics:");

    struct work_thrd_ctx_t **workers = this->workers;
    u16 total_num_port = this->user_args->num_port;
    u64 total_sent_pkts = 0, total_recv_pkts = 0, total_drop_pkts = 0;

    // summarize stats from each port
    for(u16 i = 0; i < total_num_port; i++){
        LOG_TO_SCREEN(" - Port(%d):", i);
        LOG_TO_SCREEN("\t * Sent pkts: %d", workers[i]->sent_pkts);
        LOG_TO_SCREEN("\t * Recv pkts: %d", workers[i]->recv_pkts);
        LOG_TO_SCREEN("\t * Drop pkts: %d", workers[i]->drop_pkts);
        
        total_sent_pkts += workers[i]->sent_pkts;
        total_recv_pkts += workers[i]->recv_pkts;
        total_drop_pkts += workers[i]->drop_pkts;
    }

    LOG_TO_SCREEN("Total:");
    LOG_TO_SCREEN(" * Sent pkts: %d", total_sent_pkts);
    LOG_TO_SCREEN(" * Recv pkts: %d", total_recv_pkts);
    LOG_TO_SCREEN(" * Drop pkts: %d", total_drop_pkts);
}

int 
cmd_show_ports(struct main_thrd_ctx_t *this)
{
    LOG_TO_SCREEN("Show all router ports' configuration:");
    
    port_t **ports = this->workers[0]->ports;
    u16 total_num_port = this->user_args->num_port;
    char port_ip[INET_ADDRSTRLEN];
    
    for(u16 i = 0; i < total_num_port; i++){
        inet_ntop(AF_INET, &ports[i]->ip_addr, port_ip, INET_ADDRSTRLEN);

        LOG_TO_SCREEN(" - Port(%d):", i);
        LOG_TO_SCREEN("\t * device name: %s", ports[i]->dev_name);
        LOG_TO_SCREEN("\t * MAC address: %x.%x.%x.%x.%x.%x (Real MAC: %x.%x.%x.%x.%x.%x )",
                ports[i]->mac[0], ports[i]->mac[1], ports[i]->mac[2], 
                ports[i]->mac[3], ports[i]->mac[4], ports[i]->mac[5],
                ports[i]->real_mac[0], ports[i]->real_mac[1], ports[i]->real_mac[2], 
                ports[i]->real_mac[3], ports[i]->real_mac[4], ports[i]->real_mac[5]
                );
        LOG_TO_SCREEN("\t * IP address: %s", port_ip);
    }
}

/* ==================================== help ==================================== */
int 
cmd_help_all(struct main_thrd_ctx_t *this)
{
    u8  L1_cmd_idx = 0, L2_cmd_idx = 0, L2_cmd_end = 255;

    LOG_TO_SCREEN("List all support CLI command:");
    for(L1_cmd_idx = CMD_DEBUG; L1_cmd_idx < CMD_LAST; L1_cmd_idx++){
        LOG_TO_SCREEN("* %s: ", L1_cmd[L1_cmd_idx]);
        switch(L1_cmd_idx){
            case CMD_DEBUG:
                L2_cmd_idx = CMD_DEBUG_START + 1;
                L2_cmd_end = CMD_DEBUG_END;
                break;
            case CMD_SHOW:
                L2_cmd_idx = CMD_SHOW_START + 1;
                L2_cmd_end = CMD_SHOW_END;
                break;
            case CMD_HELP:
                L2_cmd_end = 0;
                printf("\t%s ", L1_cmd[CMD_HELP]);
                printf(": %s\n", desc_router_cmd[CMD_HELP_ALL]);
                break;
        }

        // print here
        for(L2_cmd_idx; L2_cmd_idx < L2_cmd_end; L2_cmd_idx++){
            u8 iter_idx = 0;
            printf("\t%s ", L1_cmd[L1_cmd_idx]);
            while(L2_cmd[L2_cmd_idx][iter_idx] != NULL){
                printf("%s ", L2_cmd[L2_cmd_idx][iter_idx]);
                iter_idx++;
            }
            // print description here
            printf(": %s\n", desc_router_cmd[L2_cmd_idx]);
        }
    }

    
    return 0;
}
