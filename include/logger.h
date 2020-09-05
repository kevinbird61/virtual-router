#ifndef __LOGGER__
#define __LOGGER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "utils.h"

#define SHOW_LOGFILE "/tmp/virtual_router_show.log"

#define LOG_TO_SCREEN(format, info_args...)     	\
    do {             								\
		char *log_msg = 							\
			get_msg_from_args(format, ##info_args);	\
		fprintf(stdout, "%s\n", log_msg);			\
		free(log_msg);								\
    } while(0)

#define LOG_TO_FILE(format, info_args...)     		\
    do {             								\
		char *log_msg = 							\
			get_msg_from_args(format, ##info_args);	\
		FILE *logfile = fopen(SHOW_LOGFILE, "a"); 	\
		fprintf(logfile, "%s\n", log_msg);			\
		free(log_msg);								\
		fclose(logfile);							\
    } while(0)

static inline char*
get_msg_from_args(const char *info_args, ...)
{
	char *log_msg = NULL;
    va_list ap;
    va_start(ap, info_args); 
    int size = vsnprintf(log_msg, size, info_args, ap);
    va_end(ap);

    size++;
    SAVE_ALLOC(log_msg, size, char);
    va_start(ap, info_args);
    size = vsnprintf(log_msg, size, info_args, ap);
    va_end(ap);

	return log_msg;
}

#endif
