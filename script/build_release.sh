#!/bin/bash

local RESULT="1"

# Move to project top directory
pushd `dirname $0`
source ./common.sh.inc
cd ..

mkdir -p build_Release
cd build_Release

if [[ -v RAFI_WIN ]]; then
    cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 15 2017 Win64" && cmake --build . --config Release
    RESULT="$?"
else
    cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build . --config Release
    RESULT="$?"
fi

popd

exit ${RESULT}
