#!/bin/bash

OPTION=""
if [ $# -ge 1 ]; then
    OPTION="-f $1"
fi

# Move to project top directory
pushd `dirname $0`
cd ..

source script/common.sh.inc
${RAFI_PYTHON} ./tool/prepare_riscv_tests.py -i ./test/riscv_tests.config.json ${OPTION}

popd
