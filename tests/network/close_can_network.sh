device_name=$(cat ./can_device_name.txt)

sudo ip link delete $device_name
