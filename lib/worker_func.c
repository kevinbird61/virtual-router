#include "process_l2_pkt.h"
#include "router_ctx.h"
#include "ether.h"
#include "utils.h"

#include <sys/poll.h>
#include <stdio.h>

/**
 * run_port:
 * - read data from TUN/TAP client (simulate router's port)
 */
int 
run_port(struct work_thrd_ctx_t *this) 
{
    struct pollfd ufds[1];
    port_t *port = this->ports[this->port_idx];
    u16 poll_timeout = 5000; // 5 seconds
    this->debug = 1; // FIXME: set true as default (this field is configurable)
    int ret, nread;

    // init ctx
    SAVE_ALLOC(this->pkt_buff, 1500, char); // max. length = 1500 

    // using poll to control I/O events from all ports
    ufds[0].fd = port->fd;
    ufds[0].events = POLLIN; // only read

    // main loop 
    while(1) {
        if ( (ret = poll(ufds, 1, poll_timeout) ) > 0 ) { 
            // check R/W and exception IO
            // =================== error handler =================== //
            if( ufds[0].revents & POLLERR ){

            }
            // =================== able to receive =================== //
            if( ufds[0].revents & POLLIN ){
                nread = read(ufds[0].fd, this->pkt_buff, 1500);
                if(nread < 0 ){
                    perror("cannot read from tuntap");
                    break;
                }
                
                // FIXME: we assume that all start from ethernet header
                process_eth_pkt(this);
            }
            // =================== able to send =================== //
            /*if( ufds[0].revents & POLLOUT ){
                
            }*/
        } else if (ret == -1) {
            perror("poll error");
            break;
        } else {
            // timeout occur
            // perror("poll timeout");
        }
    } 

    return 0;
}
