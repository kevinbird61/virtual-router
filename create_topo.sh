#!/bin/bash

# topology:
#                               (    br0    )
# (netns: h1)[veth1] <--veth--> [veth2][tap0] <--tuntap--> (TUN/TAP client, simulate port 1 of router) ----- Software Router
#                                                                                                        ^
#                                                                                                        |
#                               (    br1    )                                                            |
# (netns: h2)[veth3] <--veth--> [veth4][tap1] <--tuntap--> (TUN/TAP client, simulate port 2 of router) ---

# create tap0
ip tuntap add mode tap dev tap0
ip tuntap add mode tap dev tap1
ip addr add 10.0.0.2/24 dev tap0
ip addr add 10.0.1.2/24 dev tap1
ip link set dev tap0 up
ip link set dev tap1 up

# create namespace
ip netns add h1
ip netns add h2

# create veth: veth1 connect to h1, and veth2 connect to bridge
ip link add veth1 netns h1 type veth peer name veth2
ip link add veth3 netns h2 type veth peer name veth4

# create bridge br0, br1 and connect with one side of veth
ip link add dev br0 type bridge
ip link add dev br1 type bridge
ip link set dev br0 up
ip link set dev veth2 master br0
ip link set dev veth2 up
ip link set dev tap0 master br0
ip link set dev br1 up
ip link set dev veth4 master br1
ip link set dev veth4 up
ip link set dev tap1 master br1

# enable veth1 in h1
ip netns exec h1 ip link set dev veth1 up
# enable veth3 in h2
ip netns exec h2 ip link set dev veth3 up

# add route for h1, h2
ip netns exec h1 ip addr add "10.0.0.1/24" dev veth1
ip netns exec h1 ip route add "10.0.0.0/16" dev veth1
ip netns exec h2 ip addr add "10.0.1.1/24" dev veth3
ip netns exec h2 ip route add "10.0.0.0/16" dev veth3
