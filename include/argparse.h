#ifndef __ARGPARSE__
#define __ARGPARSE__

#include <getopt.h>
#include <stdarg.h>

#include "logger.h"
#include "utils.h"
#include "types.h"
#include "port.h"

// user argument
typedef struct _arg_parse_t {
    u16     num_port; // how many interfaces user want to create
    port_t  **port;    
} argparse_t;

int parse_args(int argc, char *argv[], argparse_t *myargs);

/** usage: 
 *  "long_opt", "short_opt", "description"
 */
#define PRINT_USAGE()   \
    do {                \
        LOG_TO_SCREEN("*********************************"); \
        LOG_TO_SCREEN("Usage: ./router.exe [OPTIONS]"); \
        LOG_TO_SCREEN("%-4s%-s, %-s, %-s", "", "--intf", "-i", "Specify interfaces for net devices. Format: -i <port_1> <port_2> <port_3> ..."); \
        LOG_TO_SCREEN("%-4s%-s, %-s, %-s", "", "--help", "-h", "Print this help message"); \
        LOG_TO_SCREEN("*********************************"); \
        LOG_TO_SCREEN("Example: "); \
        LOG_TO_SCREEN("- create 2 interfaces, tap0 and tap1: "); \
        LOG_TO_SCREEN("     ./router.exe -i tap0 tap1"); \
        LOG_TO_SCREEN("*********************************"); \
    } while(0)


#endif
