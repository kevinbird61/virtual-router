#!/bin/bash

# delete tun/tap
ip link del dev tap0
ip link del dev tap1
# delete bridge
ip link del dev br0
ip link del dev br1
# delete veth peer
ip link del dev veth2
ip link del dev veth4
# delete namespace
ip netns del h1
ip netns del h2
