num_slaves=$1

if [[ $num_slaves < 1 ]]; then
    echo "create at lease one slave"
    exit -1
fi

cd ./slave_nodes
make clean
make
cd ..

base_n_vars=3
base_n_coms=1
for ((i = 0; i < $num_slaves; i++)); do
    ./slave_nodes/main "SLAVE$i" $[$base_n_vars+$i] $[$base_n_coms+$i] &
done

