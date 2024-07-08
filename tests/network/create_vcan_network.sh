device_name=$(cat ./can_device_name.txt)

sudo ip link add dev $device_name type vcan
sudo ip link set up $device_name
