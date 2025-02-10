#!/bin/sh

YELLOW="\e[33m"
GREEN="\e[32m"
RED="\e[31m"
MAGENTA="\e[35m"
ENDCOLOR="\e[0m"

test_root=$(pwd)

end_tests() {
    cd $test_root
    ./setup_test_env.sh "close"
}

run_test() {
    echo -e ${MAGENTA}testing $(echo $1 | cut -d'/' -f1) $ENDCOLOR
    cd $1
    echo -e ${YELLOW}building in DEBUG mode $ENDCOLOR
    mkdir build 2>/dev/null
    cd build
    rm -rf ./*
    cmake .. 1>/dev/null
    make build_debug -j8 1>/dev/null
    echo -e ${GREEN}running in DEBUG mode $ENDCOLOR
    cd debug
    ./main
    cd ..
    make clean 1>/dev/null
    echo -e ${YELLOW}building in RELEASE mode $ENDCOLOR
    make build_release -j8 1>/dev/null
    echo -e ${GREEN}running in RELEASE mode $ENDCOLOR
    cd release
    ./main
    cd ..
}

if [ $# -ge 3 -a $# -le 0 ]; then
    echo $0 [name]
    exit 1
fi

if [ ! -d ./.dummy ]; then
    echo run the script inside the test directory
    exit 2
fi

./setup_test_env.sh "init"


cd ./tests/
if [ $# -eq 1 ]; then
    if [ -d ./$1 ]; then
        run_test $1
    else
        echo test not found
    fi
    end_tests
    exit 0
fi

for TEST_DIR in $(/bin/ls -d */ 2>/dev/null ); do
    run_test $TEST_DIR
    cd $test_root/tests
done

end_tests

exit 0
