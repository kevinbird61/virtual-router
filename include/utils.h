#ifndef __UTILS__
#define __UTILS__

#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "types.h"

#define USEC    1000000

#define SAVE_ALLOC(ptr, num, type)          \
    do {                                    \
        ptr = malloc(num * sizeof(type));   \
        if(ptr == NULL){                          \
            perror("Cannot allocate memory");   \
            exit(1);                            \
        }                                       \
        memset(ptr, 0x00, num * sizeof(type));    \
    } while(0)


static inline u64 get_utime(void)
{
    struct timezone tz;
    struct timeval tvstart;
    gettimeofday(&tvstart, &tz);
    return ((tvstart.tv_sec)*USEC) + (tvstart.tv_usec);
}


#endif
