#include "process_l2_pkt.h"
#include "debugger.h"

int 
process_eth_pkt(struct work_thrd_ctx_t *sbuff)
{
    ethhdr *eth = (ethhdr *) sbuff->pkt_buff;
    sbuff->nh = sizeof(ethhdr);

    if(sbuff->debug){ 
        LOG_PKT_START();
        LOG_PKT_INFO(sbuff, LOG_ETH);
    }

    // FIXME: check if dmac is owned by us or broadcast
    // if we have virtual host, then we also need to check here.
    /*if((memcmp(eth->dmac, sbuff->ports[sbuff->port_idx]->mac, 6) != 0) 
        (memcmp(eth->dmac, bcast_mac, 6) != 0)){
        LOG_TO_SCREEN("(%d) this packet isn't for us", sbuff->port_idx);
        return -1;
    }*/

    STATS_INC_RECV_PKT(sbuff);

    // process next layer
    if( ntohs(eth->ethertype) == ETH_IP) {
        process_ip_pkt(sbuff);
    } else if( ntohs(eth->ethertype) == ETH_ARP ) {
        process_arp_pkt(sbuff);
    } else if( ntohs(eth->ethertype) == ETH_IP6 ) {
        // TODO
    }

    if (sbuff->debug) { LOG_PKT_END(); }
}
