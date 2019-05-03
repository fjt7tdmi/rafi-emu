#!/bin/bash

# Move to project top directory
pushd `dirname $0`
source ./common.sh.inc
cd ..

mkdir -p build
cd build

if [[ -v RAFI_WIN ]]; then
    cmake .. -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 15 2017 Win64" && cmake --build . --config Debug
else
    cmake .. -DCMAKE_BUILD_TYPE=Debug && cmake --build . --config Debug
fi

popd