#!/bin/bash

if [[ "$(uname)" =~ ^MINGW ]]; then
    RAFI_PYTHON=python
else
    RAFI_PYTHON=python3
fi
