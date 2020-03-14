#!/bin/bash

main()
{
    # $1 = dev name (-z: is an empty string or not)
    if [ -z $1 ]; then
        echo "Usage: ./delete_intf.sh <dev_name>"
        echo ""
    else
        # delete
        ip link del $1
    fi
}


# run main func
main $1
