#!/bin/bash

# Move to project top directory
pushd `dirname $0`
cd ..
source script/common.sh.inc

mkdir -p share
docker run -it -v $PWD/share:/share sbates130272/riscv

popd
