#!/bin/bash

OPTION=""
if [ $# -ge 1 ]; then
    OPTION="-c $1"
fi

# Move to project top directory
pushd `dirname $0`
cd ..

source script/common.sh.inc
${RAFI_PYTHON} ./tool/run_zephyr.py ${OPTION}

popd
