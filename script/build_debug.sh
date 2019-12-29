#!/bin/bash

local RESULT="1"

# Move to project top directory
pushd `dirname $0`
source ./common.sh.inc
cd ..

mkdir -p build_Debug
cd build_Debug

if [[ -v RAFI_WIN ]]; then
    cmake .. -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 15 2017 Win64" && cmake --build . --config Debug
    RESULT="$?"
else
    cmake .. -DCMAKE_BUILD_TYPE=Debug && cmake --build . --config Debug
    RESULT=$?
fi

popd

exit ${RESULT}
