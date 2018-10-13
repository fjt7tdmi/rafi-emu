#!/bin/bash

# Move to project top directory
pushd `dirname $0`
cd ..

mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

popd