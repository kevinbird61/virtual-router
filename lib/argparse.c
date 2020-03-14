#include "argparse.h"

int
parse_args(int argc, char *argv[], argparse_t *myargs)
{
    int ch = 0, opt_idx = 0;
    const char *short_opt = "hi:";
    port_t *port = NULL;
    
    struct option long_opt[] = 
    {
        {"intf", required_argument, NULL, 'i'},
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0}
    };

    while((ch = getopt_long_only(argc, argv, short_opt, long_opt, &opt_idx)) != -1)
    {
        switch(ch)
        {
            case 'i':{
                optind--;
                int num_intf = 0, start_idx = optind;
                // get the number of total interfaces
                for(; start_idx < argc && *argv[start_idx] != '-'; start_idx++){
                    num_intf++; 
                }
                myargs->num_port = num_intf;
                start_idx = 0;
                // 2d array alloc
                SAVE_ALLOC(myargs->port, num_intf, port_t **);
                // init those interfaces
                for(; optind < argc && *argv[optind] != '-'; optind++){
                    // alloc and reuse start_idx
                    SAVE_ALLOC(myargs->port[start_idx], 1, port_t *);
                    SAVE_ALLOC(myargs->port[start_idx]->dev_name, strlen(argv[optind]), u8);
                    // get device name
                    sscanf(argv[optind], "%s", myargs->port[start_idx]->dev_name);
                    start_idx++;
                }
                break;
            }
            default:
                // print helper function
                PRINT_USAGE();
                exit(1);
        }
    }

    return 0;
}