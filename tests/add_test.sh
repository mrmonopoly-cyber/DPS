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

if [ -d ./tests/$name ]; then
    echo "test already present"
fi

cp -r ./.dummy ./tests/$name
