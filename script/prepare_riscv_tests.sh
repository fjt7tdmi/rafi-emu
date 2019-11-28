#!/bin/bash

# Move to project top directory
pushd `dirname $0`
cd ..

source script/common.sh.inc

${RAFI_PYTHON} ./tool/prepare_riscv_tests.py -t isa -o ./work/riscv-tests -i ./test/riscv_tests.config.json
${RAFI_PYTHON} ./tool/prepare_riscv_tests.py -t benchmarks -o ./work/riscv-benchmarks -i ./test/riscv_benchmarks.config.json

popd
