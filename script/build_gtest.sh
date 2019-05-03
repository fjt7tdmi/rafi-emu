#!/bin/bash

# Move to project top directory
pushd `dirname $0`
source ./common.sh.inc
cd ../third_party/googletest

mkdir -p build
cd build

if [[ -v RAFI_WIN ]]; then
    cmake .. -DBUILD_GTEST=ON -DBUILD_GMOCK=OFF -G "Visual Studio 15 2017 Win64" && cmake --build . --config Debug
else
    cmake .. -DBUILD_GTEST=ON -DBUILD_GMOCK=OFF && cmake --build . --config Debug
fi

popd