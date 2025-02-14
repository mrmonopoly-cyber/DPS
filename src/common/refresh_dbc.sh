#!/bin/sh

if [[ ! -e ./common.h ]]; then
  echo "usage ${0}. Run the script in src/common"
  exit 1
fi

cd ../../lib/dbcc

make

cd -

../../lib/dbcc/dbcc ../../dps_mesages.dbc

