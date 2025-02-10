#!/bin/sh

if [[ $# -ne 0 ]]; then
    echo $0
    exit 1
fi

if [ ! -d ./.dummy -o ! -d ./tests ]; then
    echo run the script inside the test directory
    exit 2
fi


for TEST in $(ls -A ./tests);do
    echo "- $TEST"
done
