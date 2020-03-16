#include "process_l2_pkt.h"

int 
process_eth_pkt(struct work_thrd_ctx_t *sbuff)
{
    ethhdr *eth = (ethhdr *) sbuff->pkt_buff;
    sbuff->nh = sizeof(ethhdr);

    if(sbuff->debug){ 
        LOG_TO_SCREEN("=========================================================");
        LOG_TO_SCREEN("(%d) DMAC: %x.%x.%x.%x.%x.%x , SMAC: %x.%x.%x.%x.%x.%x", sbuff->port_idx
                , eth->dmac[0], eth->dmac[1], eth->dmac[2], eth->dmac[3], eth->dmac[4], eth->dmac[5]
                , eth->smac[0], eth->smac[1], eth->smac[2], eth->smac[3], eth->smac[4], eth->smac[5]);
        LOG_TO_SCREEN("(%d) ethertype: %s", sbuff->port_idx, g_ethertype_str[ntohs(eth->ethertype)]);
    }

    // FIXME: check if dmac is owned by us or broadcast
    // if we have virtual host, then we also need to check here.
    /*if((memcmp(eth->dmac, sbuff->ports[sbuff->port_idx]->mac, 6) != 0) 
        (memcmp(eth->dmac, bcast_mac, 6) != 0)){
        LOG_TO_SCREEN("(%d) this packet isn't for us", sbuff->port_idx);
        return -1;
    }*/

    // process next layer
    if( ntohs(eth->ethertype) == ETH_IP) {
        process_ip_pkt(sbuff);
    } else if( ntohs(eth->ethertype) == ETH_ARP ) {
        process_arp_pkt(sbuff);
    } else if( ntohs(eth->ethertype) == ETH_IP6 ) {
        // TODO
    }
}
