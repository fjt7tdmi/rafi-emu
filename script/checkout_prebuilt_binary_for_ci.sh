#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 github_user github_access_token"
    exit 1
fi

user=$1
token=$2

# Move to project top directory
pushd `dirname $0`
cd ..

source script/common.sh.inc

cd third_party 

git clone https://${user}:${token}@github.com/fjt7tdmi/rafi-prebuilt-binary.git

popd
