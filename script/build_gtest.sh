#!/bin/bash

local RESULT= 1

# Move to project top directory
pushd `dirname $0`
source ./common.sh.inc
cd ../third_party/googletest

if [[ -v RAFI_WIN ]]; then
    # Debug
    mkdir -p x64-Debug
    pushd x64-Debug
        cmake .. -DBUILD_GTEST=ON -DBUILD_GMOCK=OFF -G "Visual Studio 15 2017 Win64" && cmake --build . --config Debug
        RESULT="$?"
    popd

    # Release
    mkdir -p x64-Release
    pushd x64-Release
        cmake .. -DBUILD_GTEST=ON -DBUILD_GMOCK=OFF -G "Visual Studio 15 2017 Win64" && cmake --build . --config Release
        RESULT="$?"
    popd
else
    mkdir -p build
    pushd build
        cmake .. -DBUILD_GTEST=ON -DBUILD_GMOCK=OFF && cmake --build .
        RESULT="$?"
    popd
fi

popd

exit ${RESULT}
