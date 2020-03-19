#include "router_ctx.h"

int 
add_route(
    u32 ip, u32 netmask, u32 gateway, port_t *port)
{
    u8  is_full = 1;

    for(u8 i = 0; i < ROUTE_TABLE_SIZE; i++){
        if(!route_table[i].is_used){
            route_table[i].is_used = 1;
            route_table[i].ip = ip;
            route_table[i].netmask = netmask;
            route_table[i].gw = gateway;
            route_table[i].port = port;
            is_full = 0;
            break;
        }
    }

    if(is_full){
        return -1;
    }

    return 0;
}

port_t *
lookup_route(u32 target_ip)
{
    for(u8 i = 0; i < ROUTE_TABLE_SIZE; i++){
        if(route_table[i].is_used){
            if((ntohl(route_table[i].ip) & route_table[i].netmask) == 
                (ntohl(target_ip) & route_table[i].netmask))
            {
                // match 
                return route_table[i].port;
            }
        }
    }

    return NULL;
}
