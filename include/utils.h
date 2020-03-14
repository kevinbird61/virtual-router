#ifndef __UTILS__
#define __UTILS__

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>

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


#endif