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

#include <map>
#include <memory>
#include <type_traits>

#include <rafi/emu.h>

#include "../System.h"

namespace rafi { namespace emu {

template <typename T>
void BinaryToHex(char* pOutBuffer, size_t bufferSize, const T value)
{
    (void)bufferSize; // for release build

    static_assert(std::is_integral_v<T>);
    static_assert(std::is_unsigned_v<T>);

    auto tmp = value;

    for (int i = 0; i < sizeof(tmp) * 2; i += 2)
    {
        const T high = (tmp % 0x100) / 0x10;
        const T low = tmp % 0x10;

        assert(i + 1 <= bufferSize);

        pOutBuffer[i] = static_cast<char>(high < 10 ? '0' + high : 'a' + (high - 10));
        pOutBuffer[i + 1] = static_cast<char>(low < 10 ? '0' + low : 'a' + (low - 10));

        tmp >>= 8;
    }
}

void StringToHex(char* pOutBuffer, size_t bufferSize, const char* str);
void StringToHex(char* pOutBuffer, size_t bufferSize, const std::string str);

}}
