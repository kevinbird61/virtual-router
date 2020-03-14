#ifndef __PORT__
#define __PORT__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>         // close
#include <fcntl.h>
#include <net/if.h>         // ifreq
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/if_tun.h>

#include "types.h"

/* create TAP/TUN for ports,interfaces, return file descriptor */
int create_port(char *dev, int flags);
int get_mac_addr(port_t *port);

#endif
