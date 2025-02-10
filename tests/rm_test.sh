#!/bin/sh

name=$1

if [[ $# -ne 1 ]]; then
    echo $0 [name]
    exit 1
fi

if [ ! -d ./.dummy -o ! -d ./tests ]; then
    echo run the script inside the test directory
    exit 2
fi

rm -rf ./tests/$name
