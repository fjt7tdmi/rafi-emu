#!/bin/bash

build_type="Debug"
if [ $# -ne 0 ]; then
    build_type=$1
fi

# Move to project top directory
pushd `dirname $0`
cd ..

source script/common.sh.inc


if [[ "$(uname)" =~ ^MINGW ]]; then
    ./build_${build_type}/${build_type}/rafi-unit-test
else
    ./build_${build_type}/rafi-unit-test
fi

exit_code=$?

popd

exit ${exit_code}