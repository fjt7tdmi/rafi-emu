#!/bin/bash

# Move to project top directory
pushd `dirname $0`
cd ../third_party/googletest

mkdir -p build
cd build

cmake .. -DBUILD_GTEST=ON -DBUILD_GMOCK=OFF
cmake --build .

popd