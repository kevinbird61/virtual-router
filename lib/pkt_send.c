#include "router_ctx.h"

int 
pkt_send(port_t *port, u8 *pkt_buff, u16 len)
{
    int nwrite = 0;
    nwrite = write(port->fd, pkt_buff, len);
    return nwrite;
}
