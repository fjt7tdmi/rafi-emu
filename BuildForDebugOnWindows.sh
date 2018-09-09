#!/bin/bash

# Move to project top directory
pushd `dirname $0`

mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

popd