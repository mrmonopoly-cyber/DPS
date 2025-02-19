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

if [ ! -d ./tests -o ! -d ./.dummy ]; then
    echo run the script inside the test directory
    exit 2
fi

skip_tests=""

help ()
{
  echo "usage ${0}:
    -h        : print help
    -s args.. : skip a list of test 
    -t arg    : run a specific test
    --all     : run all tests sequentially
    
    If no argument is passed all test are runned sequentially"
}

case "$1" in
  "-h") 
    help
    exit 0
  ;;
  "-s") 
    echo -n "skipping tests: ["
    shift
    skip_tests=${@}
    for ARG in "$@"
    do
      echo -n " $ARG "
    done
    echo "]"
  ;;
  "-t") echo "single test"
    ./setup_test_env.sh "init"
    cd ./tests/
    shift
    if [ $# -eq 1 ]; then
      if [ -d ./$1  ]; then
        run_test $1
      else
        echo test not found
      fi
      end_tests
    fi
    exit 0
  ;;
  "--all") echo "running all tests"
  ;;
  *) 
    help
    exit 0
  ;;
esac



./setup_test_env.sh "init"
cd tests
for TEST_DIR in $(/bin/ls -d */ 2>/dev/null ); do
  curr=$(echo $TEST_DIR| cut -d'/' -f1)
  if [[ -z $(echo "$skip_tests" | grep -w "${curr}") ]]; then
    run_test $TEST_DIR
  else
    echo -e ${RED}skipping $(echo ${curr} | cut -d'/' -f1) $RED
  fi
  cd $test_root/tests
done

end_tests

exit 0
