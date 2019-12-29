#!/bin/bash

# Move to project top directory
pushd `dirname $0`
cd ..

source script/common.sh.inc

cd third_party 

# Sorry, the prebuilt binary repository is private now.
# We need to clear licenses for redistribution.
git clone git@github.com:fjt7tdmi/rafi-prebuilt-binary.git

popd
