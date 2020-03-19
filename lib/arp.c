#include "router_ctx.h"
#include "arph.h"

/* ARP operations */
int 
add_new_arp_cache(struct work_thrd_ctx_t *sbuff)
{
    u8 is_full = 1;
    u16 port_idx = sbuff->port_idx;
    arphdr *arph = (arphdr *) (sbuff->pkt_buff + sbuff->nh);

    pthread_spin_trylock(&arp_lock);
    // update arp table
    for(u8 i = 0; i < ARP_TABLE_SIZE; i++){
        if(arp_tb[i].state == AS_FREE){
            // FIXME: other arp entry's info
            arp_tb[i].state = AS_INCOMPLETE;
            arp_tb[i].ip = arph->arp_ip_daddr;
            is_full = 0;
            break;
        } else if(arp_tb[i].state == AS_INCOMPLETE){
            if((arp_tb[i].ip == arph->arp_ip_daddr)){
                // already added, pass 
                is_full = 0;
                break;
            }    
        }
    }
    // after lookup and update, release the lock
    pthread_spin_unlock(&arp_lock);

    if(is_full){
        LOG_TO_SCREEN("(%d) ARP cache is full, need to drop some old entry.", sbuff->port_idx);
        return -1;
    }

    return 0;
}

int 
resolve_arp_cache(struct work_thrd_ctx_t *sbuff)
{
    u8  found = 0, entry_idx = 0;
    u16 port_idx = sbuff->port_idx;
    arphdr *arph = (arphdr *) (sbuff->pkt_buff + sbuff->nh);
    port_t *port = sbuff->ports[port_idx];

    // update arp table
    pthread_spin_trylock(&arp_lock);
    for(u8 i = 0; i < ARP_TABLE_SIZE; i++){
        if(arp_tb[i].state == AS_INCOMPLETE && arp_tb[i].ip == arph->arp_ip_saddr){
            // match
            arp_tb[i].state = AS_RESOLVED;
            memcpy(arp_tb[i].mac, arph->arp_eth_src, 6);
            arp_tb[i].ip = arph->arp_ip_saddr;
            arp_tb[i].port = port;
            LOG_TO_SCREEN("(%d) Resolved an ARP entry, idx = %d", port_idx, i);
            found = 1;
            entry_idx = i;
            break;
        }
    }
    pthread_spin_unlock(&arp_lock);
}

int 
fill_dmac_by_arp(u32 ip, u8 *mac)
{
    struct arp_entry entry;
    for(u8 i = 0; i < ARP_TABLE_SIZE; i++){
        entry = arp_tb[i];
        if(entry.state == AS_RESOLVED && entry.ip == ip){
            memcpy(mac, entry.mac, 6);
            return 0;
        }
    }

    return -1;
}