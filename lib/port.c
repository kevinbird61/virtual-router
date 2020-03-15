#include "port.h"

int 
create_port(char *dev, int flags) {

    struct ifreq ifr;
    int fd, err;
    char *clonedev = "/dev/net/tun";

    /* Arguments taken by the function:
    *
    * char *dev: the name of an interface (or '\0'). MUST have enough
    *   space to hold the interface name if '\0' is passed
    * int flags: interface flags (eg, IFF_TUN etc.)
    */

    /* open the clone device */
    if( (fd = open(clonedev, O_RDWR)) < 0 ) {
        perror("Cannot open /dev/net/tun");
        return fd;
    }

   /* preparation of the struct ifr, of type "struct ifreq" */
   memset(&ifr, 0, sizeof(ifr));

   ifr.ifr_flags = flags;   /* IFF_TUN or IFF_TAP, plus maybe IFF_NO_PI */

    if (*dev) {
        /* if a device name was specified, put it in the structure; otherwise,
        * the kernel will try to allocate the "next" device of the
        * specified type */
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    /* try to create the device */
    if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
        perror("Failed to create this device");
        close(fd);  
        return err;
    }

    /* if the operation was successful, write back the name of the
    * interface to the variable "dev", so the caller can know
    * it. Note that the caller MUST reserve space in *dev (see calling
    * code below) */
    strcpy(dev, ifr.ifr_name);

    /* this is the special file descriptor that the caller will use to talk
    * with the virtual interface */
    return fd;
}

int
get_dev_info(port_t *port)
{
    int sockfd;
    struct ifreq ifr, ifr_ip;
    strcpy(ifr.ifr_name, port->dev_name);
    
    /* get mac addr */
    if(ioctl(port->fd, SIOCGIFHWADDR, &ifr) < 0){
        perror("cannot get mac addr from port");
        exit(1);
    }
    memcpy(port->mac, ifr.ifr_hwaddr.sa_data, 6);
    
    /* get ip addr, need to create socket first (for SIOCGIFADDR) */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("cannot initialize sockfd");
        exit(1);
    }
    if(ioctl(sockfd, SIOCGIFADDR, &ifr) < 0 ){
        perror("cannot get ip addr from port");
        exit(1);
    }
    port->ip_addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
}
