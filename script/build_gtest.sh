#!/bin/bash

exit_code="1"

# Move to project top directory
pushd `dirname $0`
source ./common.sh.inc
cd ../third_party/googletest

if [[ -v RAFI_WIN ]]; then
    # Debug
    mkdir -p x64-Debug
    pushd x64-Debug
        cmake .. -DBUILD_GTEST=ON -DBUILD_GMOCK=OFF -G "Visual Studio 15 2017 Win64" && cmake --build . --config Debug
        exit_code="$?"
    popd

    # Release
    mkdir -p x64-Release
    pushd x64-Release
        cmake .. -DBUILD_GTEST=ON -DBUILD_GMOCK=OFF -G "Visual Studio 15 2017 Win64" && cmake --build . --config Release
        exit_code="$?"
    popd
else
    mkdir -p build
    pushd build
        cmake .. -DBUILD_GTEST=ON -DBUILD_GMOCK=OFF && cmake --build .
        exit_code="$?"
    popd
fi

popd

exit ${exit_code}
