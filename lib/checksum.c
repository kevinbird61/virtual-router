#include "checksum.h"

u16 
calc_icmp_cksum(u16 *icmph, int len)
{
    u16 ret = 0;
    u32 sum = 0xffff;

    while(len > 1){
        sum += *icmph++;
        len -= 2;
    }

    if(len == 1){
        sum += *(u8*)icmph;
    }

    sum = (sum & 0xffff) + (sum >> 16);
    sum += (sum >> 16);
    
    return ~sum;
}
