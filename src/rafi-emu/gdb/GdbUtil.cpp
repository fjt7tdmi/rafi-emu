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

#include <string>

#include <rafi/emu.h>

#include "GdbUtil.h"

namespace rafi { namespace emu {

std::string StringToHex(const std::string& str)
{
    std::string response;

    for (int i = 0; i < str.size(); i++)
    {
        const auto x = static_cast<uint8_t>(str[i]);

        response += BinaryToHex(x);
    }

    return response;
}

uint8_t HexCharToUInt8(char c)
{
    if ('0' <= c && c <= '9')
    {
        return static_cast<uint8_t>(c - '0');
    }
    else if ('a' <= c && c <= 'f')
    {
        return static_cast<uint8_t>(c - 'a' + 10);
    }
    else
    {
        printf("[gdb] input is not hex.\n");
        RAFI_NOT_IMPLEMENTED;
    }
}

uint8_t HexToUInt8(const std::string& str)
{
    return static_cast<uint8_t>(HexToUInt64(str));
}

uint64_t HexToUInt64(const std::string& str)
{
    uint64_t sum = 0;

    for (size_t i = 0; i < str.size(); i++)
    {
        sum <<= 4;
        sum += HexCharToUInt8(str[i]);
    }

    return sum;
}

}}
