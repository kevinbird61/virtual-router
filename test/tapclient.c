#include "ether.h"
#include "port.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* tapclient */
int main(int argc, char *argv[])
{
    char tun_name[IFNAMSIZ];
  
    /* Connect to the device */
    strcpy(tun_name, argv[1]);
    int tun_fd = create_port(tun_name, IFF_TAP | IFF_NO_PI);  /* IFF_NO_PI: contain additional 4 bytes info */

    if(tun_fd < 0){
        perror("Allocating interface");
        exit(1);
    }
    puts("Finished.");

    /* Now read data coming from the kernel */
    char buffer[256];
    ethhdr *eth;

    while(1) {
        /* Note that "buffer" should be at least the MTU size of the interface, eg 1500 bytes */
        int nread, nwrite;
        nread = read(tun_fd, buffer, sizeof(buffer));
        if(nread < 0) {
            perror("Reading from interface");
            close(tun_fd);
            exit(1);
        }
        /* Do whatever with the data */
        // printf("Read %d bytes from device %s\n", nread, tun_name);
        eth = (ethhdr *)buffer;

        puts("-----------------------------------");
        printf("dmac: %x.%x.%x.%x.%x.%x\n", eth->dmac[0], eth->dmac[1], eth->dmac[2],
            eth->dmac[3], eth->dmac[4], eth->dmac[5]);
        printf("smac: %x.%x.%x.%x.%x.%x\n", eth->smac[0], eth->smac[1], eth->smac[2],
            eth->smac[3], eth->smac[4], eth->smac[5]);
        printf("ethertype: %s\n", g_ethertype_str[ntohs(eth->ethertype)]);

        // write dummy data (test tun/tap)
        /*
        nwrite = write(tun_fd, buffer, nread);
        if(nwrite < 0){
           printf("Cannot send!\n"); 
        } 
        printf("Write %d bytes to device %s\n", nwrite, tun_name);
        */
    }
}