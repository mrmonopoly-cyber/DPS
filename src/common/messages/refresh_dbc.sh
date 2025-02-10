#!/bin/sh

if [[ ! -e ./dps_template.dbc ]]; then
  echo "usage ${0}. Run the script in src/common/messages"
  exit 1
fi

cd ../../../lib/dbcc

make

cd -

../../../lib/dbcc/dbcc ./dps_template.dbc

