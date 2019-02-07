/*
 * Copyright 2018 Akifumi Fujita
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cstdio>
#include <string>
#include <iostream>

#include "Exception.h"

#define RAFI_EMU_ERROR(...) \
    do { \
        fprintf(stderr, "[ERROR] Emulator error @ %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        throw RafiEmuException(); \
    } while(0)

#define RAFI_EMU_NOT_IMPLEMENTED() \
    do { \
        fprintf(stderr, "[ERROR] Not implemented @ %s:%d\n", __FILE__, __LINE__); \
        throw RafiEmuException(); \
    } while(0)

#define RAFI_EMU_CHECK_ACCESS(_address, _size, _capacity) \
    do { \
        auto _address0 = (_address); \
        auto _size0 = (_size); \
        auto _capacity0 = (_capacity); \
        if (0 <= _address0 && _address0 + _size0 - 1 < _capacity0)  \
        { \
            fprintf(stderr, "[ERROR] Access check failure @ %s:%d\n", __FILE__, __LINE__); \
            std::cerr << std::hex << "address: 0x" << _address0 << ", size:0x" << _size0 << ", capacity:0x" << _capacity0 << std::endl; \
            throw RafiEmuException(); \
        } \
    } while(0)

#define RAFI_EMU_CHECK_RANGE(_lowerBound, _actual, _upperBound) \
    do { \
        auto _lowerBound0 = (_lowerBound); \
        auto _actual0 = (_actual); \
        auto _upperBound0 = (_upperBound); \
        if ((_actual0) < (_lowerBound0) || (_upperBound0) < (_actual0))  \
        { \
            fprintf(stderr, "[ERROR] Range check failure @ %s:%d\n", __FILE__, __LINE__); \
            std::cerr << std::hex << "lower: 0x" << _lowerBound0 << ", actual:0x" << _actual0 << ", upper:0x" << _upperBound0 << std::endl; \
            throw RafiEmuException(); \
        } \
    } while(0)
