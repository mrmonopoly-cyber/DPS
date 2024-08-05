#!/bin/sh


if [[ $# != 1 ]]
then
    echo "Usage: $0 one new type to create"
    exit 1
fi

new_type=${1}
new_type_path=./can_mex/${1}.h


echo "creating new type ${new_type}"
cp ./can_mex/dummy_mex.h $new_type_path
sed -i 's/DUMMY_NAME/'$new_type'/' $new_type_path
v=$(echo $new_type | tr a-z A-Z)
sed -i 's/DUMMY/'$v'/' $new_type_path


echo "adding type to the message index"
mex_types=./can_mex/base_mex_components/mex_types.h
new_line=$(cat $mex_types | grep -n "DUMMY_TYPE" | cut -d':' -f1)
new_index=$(echo $new_type | tr a-z A-Z)
sed -i $[$new_line-1]'a \'$(echo $1"," | tr a-z A-Z)'' $mex_types
