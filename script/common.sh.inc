#!/bin/bash

if [[ "$(uname)" =~ ^MINGW ]]; then
    export RAFI_PYTHON=python
    export RAFI_WIN=1
else
    export RAFI_PYTHON=python3
    unset RAFI_WIN
fi

if [[ "$(uname)" =~ Darwin ]]; then
    export CC=/usr/local/bin/gcc-8
    export CXX=/usr/local/bin/g++-8
fi
