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

#include "../ISystem.h"

namespace rafi { namespace emu {

template <typename T>
char DigitToHexChar(const T value)
{
    static_assert(std::is_integral_v<T>);

    assert(0 <= value && value < 16);

    return static_cast<char>(value < 10 ? '0' + value : 'a' + (value - 10));
}

template <typename T>
std::string BinaryToHex(const T value)
{
    static_assert(std::is_integral_v<T>);
    static_assert(std::is_unsigned_v<T>);

    std::string response;

    auto tmp = value;

    for (int i = 0; i < sizeof(tmp); i ++)
    {
        const T high = (tmp % 0x100) / 0x10;
        const T low = tmp % 0x10;

        response += static_cast<char>(DigitToHexChar(high));
        response += static_cast<char>(DigitToHexChar(low));

        tmp >>= 8;
    }

    return response;
}

std::string StringToHex(const std::string& str);

uint8_t HexCharToUInt8(char c);

uint8_t HexToUInt8(const std::string& str);

uint64_t HexToUInt64(const std::string& str);

}}
