#!/bin/sh

if [ $# -eq 0 ]; then
    echo "usage $0 [init,close]"
    exit 0
fi

dps_vcan="dps_vcan"

create() {
    sudo ip link add dev $1 type vcan
    sudo ip link set up $1
}

close() {
    sudo ip link delete $1
}

if [ $1 = "init" ]; then
    create ${dps_vcan}
fi

if [ $1 = "close" ]; then
    close ${dps_vcan}
fi
