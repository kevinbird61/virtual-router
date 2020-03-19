#!/bin/bash

create_tap()
{
    # $1 = dev name
    # $2 = dev's ip address
    # $3 = dev's ip netmask

    # create device
    ip tuntap add mode tap dev $1
    # give ip address for this device
    ip addr add $2/$3 dev $1
    # enable
    ip link set dev $1 up
}

create_tun()
{
    # $1 = dev name
    # $2 = dev's ip address
    # $3 = dev's ip netmask

    # create device
    ip tuntap add mode tun dev $1
    # give ip address for this device
    ip addr add $2/$3 dev $1
    # enable
    ip link set dev $1 up
}


main()
{
    # $1 = dev name
    # $2 = mode
    # $3 = dev's ip
    # $4 = dev's netmask
    if [[ ("$2" == "tap") || ("$2" == "TAP") ]]; then
        create_tap $1 $3 $4
        # print out information
        ip link show $1
    elif [[ ("$2" == "tun") || ("$2" == "TUN") ]]; then
        create_tun $1 $3 $4
        # print out information
        ip link show $1
    else
        echo "Usage: ./create_intf.sh <dev_name> <mode> <IP> <netmask>"
        echo ""
    fi
}


# run main func
main $1 $2 $3 $4
