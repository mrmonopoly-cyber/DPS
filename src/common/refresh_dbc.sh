#!/bin/sh

if [[ ! -e ./common.h ]]; then
  echo "usage ${0}. Run the script in src/common"
  exit 1
fi

git pull --recurse-submodules=yes
git submodule update --init --recursive --rebase --remote

cd ../../lib/dbcc

make clean
make

cd -

../../lib/dbcc/dbcc -s ../../dps_messages.dbc
