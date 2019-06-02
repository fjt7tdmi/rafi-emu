#!/bin/bash

# Move to project top directory
pushd `dirname $0`
cd ..
source script/common.sh.inc

cd docker

if [[ "$(uname)" =~ ^MINGW ]]; then
    winpty /c/Program\ Files/Docker/Docker/Resources/bin/docker build -t rafi_test:1 .
else
    docker build -t rafi_test:1 .
fi

popd
