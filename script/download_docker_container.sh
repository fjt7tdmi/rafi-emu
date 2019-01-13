#!/bin/bash

# Move to project top directory
pushd `dirname $0`
cd ..
source script/common.sh.inc

if [[ "$(uname)" =~ ^MINGW ]]; then
    winpty /c/Program\ Files/Docker/Docker/Resources/bin/docker pull sbates130272/riscv
else
    docker pull sbates130272/riscv
fi

popd
