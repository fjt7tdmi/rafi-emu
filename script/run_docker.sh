#!/bin/bash

# Move to project top directory
pushd `dirname $0`
cd ..
source script/common.sh.inc

mkdir -p share
if [[ "$(uname)" =~ ^MINGW ]]; then
    winpty /c/Program\ Files/Docker/Docker/Resources/bin/docker run -it -v $PWD/share:/share sbates130272/riscv
else
    docker run -it -v $PWD/share:/share sbates130272/riscv
fi

popd
