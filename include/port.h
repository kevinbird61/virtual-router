#ifndef __PORT__
#define __PORT__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>         // close
#include <fcntl.h>
#include <net/if.h>         // ifreq
#include <ifaddrs.h>        // getifaddrs
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_tun.h>

#include "types.h"

// port 
typedef struct _port_t {
    u8                      *dev_name;
    u8                      mac[6];
    u8                      real_mac[6];
    u16                     idx;
    u32                     ip_addr;
    u32                     fd;
} port_t;

/* create TAP/TUN for ports,interfaces, return file descriptor */
int create_port(char *dev, int flags);
int get_dev_info(port_t *port);

#endif
