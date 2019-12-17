#!/bin/bash

# Move to project top directory
pushd `dirname $0`
cd ..

source script/common.sh.inc


if [[ "$(uname)" =~ ^MINGW ]]; then
    ./build_Debug/Debug/rafi-unit-test
else
    ./build_Debug/rafi-unit-test
fi

exit_code=$?

popd

exit ${exit_code}