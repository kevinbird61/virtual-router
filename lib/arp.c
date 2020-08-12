#include "router_ctx.h"
#include "ether.h"
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
        if(sbuff->debug){
            LOG_TO_SCREEN("(%d) ARP cache is full, need to drop some old entry.", sbuff->port_idx);
        }
        return -1;
    }

    return 0;
}

int 
resolve_arp_cache(struct work_thrd_ctx_t *sbuff)
{
    u8  entry_idx = 0;
    u16 port_idx = sbuff->port_idx;
    arphdr *arph = (arphdr *) (sbuff->pkt_buff + sbuff->nh);
    port_t *port = sbuff->ports[port_idx];

    // update arp table
    pthread_spin_trylock(&arp_lock);
    for(entry_idx = 0; entry_idx < ARP_TABLE_SIZE; entry_idx++){
        if(arp_tb[entry_idx].state == AS_INCOMPLETE && 
            arp_tb[entry_idx].ip == arph->arp_ip_saddr){
            // match
            arp_tb[entry_idx].state = AS_RESOLVED;
            memcpy(arp_tb[entry_idx].mac, arph->arp_eth_src, 6);
            arp_tb[entry_idx].ip = arph->arp_ip_saddr;
            arp_tb[entry_idx].port = port;
            break;
        }
    }
    pthread_spin_unlock(&arp_lock);

    if(sbuff->debug){
        LOG_TO_SCREEN("(%d) Resolved an ARP entry, idx = %d", port_idx, entry_idx);
    }
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


int 
send_gratuitous_arp(struct work_thrd_ctx_t *sbuff)
{
    u16 port_idx = sbuff->port_idx;
    port_t *port = sbuff->ports[port_idx];
    ethhdr *eth = (ethhdr *) (sbuff->pkt_buff);
    sbuff->nh = sizeof(ethhdr);
    arphdr *arph = (arphdr *) (sbuff->pkt_buff + sbuff->nh);
    int nwrite = 0;
    
    memcpy(eth->smac, port->mac, 6);
    memcpy(eth->dmac, bcast_mac, 6);
    eth->ethertype = htons(ETH_ARP);
    
    arph->arp_hard_type = htons(0x0001); // Ethernet
    arph->arp_hard_size = 6;
    arph->arp_proto_type = htons(ETH_IP); 
    arph->arp_proto_size = 4;
    memcpy(arph->arp_eth_src, port->mac, 6);
    memcpy(arph->arp_eth_dst, bcast_mac, 6);
    arph->arp_ip_saddr = port->ip_addr;
    arph->arp_ip_daddr = port->ip_addr;
    arph->arp_op = htons(ARP_OP_RES);

    nwrite = pkt_send(port, sbuff->pkt_buff, sbuff->nh + sizeof(arphdr));

    if (nwrite) {
        STATS_INC_SENT_PKT(sbuff);
        if (sbuff->debug) {
            LOG_TO_SCREEN("(%d) Send Gratuitous ARP.", port_idx);
        }
        return 1;
    } else {
        STATS_INC_DROP_PKT(sbuff);
        return -1;
    }
}
