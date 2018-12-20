#!/bin/bash

if [[ "$(uname)" =~ ^MINGW ]]; then
    RAFI_PYTHON=python
else
    RAFI_PYTHON=python3
fi

if [[ "$(uname)" =~ Darwin ]]; then
    export CC=/usr/local/bin/gcc-8
    export CXX=/usr/local/bin/g++-8
fi
