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

void StringToHex(char* pOutBuffer, size_t bufferSize, const char* str)
{
    (void)bufferSize; // for release build

    for (int i = 0; i < std::strlen(str); i++)
    {
        const char high = (str[i] % 0x100) / 0x10;
        const char low = str[i] % 0x10;

        assert(i * 2 + 1 <= bufferSize);

        pOutBuffer[i * 2] = high < 10 ? '0' + high : 'a' + (high - 10);
        pOutBuffer[i * 2 + 1] = low < 10 ? '0' + low : 'a' + (low - 10);
    }
}

void StringToHex(char* pOutBuffer, size_t bufferSize, const std::string str)
{
    StringToHex(pOutBuffer, bufferSize, str.c_str());
}

}}
