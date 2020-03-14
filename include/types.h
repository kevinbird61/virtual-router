#ifndef __TYPES__
#define __TYPES__

#include <unistd.h>         // close
#include <fcntl.h>
#include <net/if.h>         // ifreq
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/if_tun.h>

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long   u64;

// port 
typedef struct _port_t {
    u8      *dev_name;
    u8      mac[6];
    u32     fd;
} port_t;

// user argument
typedef struct _arg_parse_t {
    u16     num_port; // how many interfaces user want to create
    port_t  **port;    
} argparse_t;

#endif
