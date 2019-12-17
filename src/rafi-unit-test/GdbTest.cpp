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

#include <sstream>

#pragma warning(push)
#pragma warning(disable : 4389)
#include <gtest/gtest.h>
#pragma warning(pop)

#include <rafi/trace.h>

#include "../../src/rafi-emu/gdb/GdbUtil.h"

namespace rafi { namespace trace {

TEST(GdbTest, BinaryToHex)
{
    auto test = [](const char* expected, const auto value)
    {
        char buffer[256] = {0};
        rafi::emu::BinaryToHex(buffer, sizeof(buffer), value);
        ASSERT_STREQ(expected, buffer);
    };

    test("00", static_cast<uint8_t>(0x00));
    test("01", static_cast<uint8_t>(0x01));
    test("ff", static_cast<uint8_t>(0xff));

    test("0000", static_cast<uint16_t>(0x0000));
    test("2301", static_cast<uint16_t>(0x0123));
    test("ffff", static_cast<uint16_t>(0xffff));

    test("00000000", static_cast<uint32_t>(0x0000'0000));
    test("67452301", static_cast<uint32_t>(0x0123'4567));
    test("ffffffff", static_cast<uint32_t>(0xffff'ffff));

    test("0000000000000000", static_cast<uint64_t>(0x0000'0000'0000'0000));
    test("efcdab8967452301", static_cast<uint64_t>(0x0123'4567'89ab'cdef));
    test("ffffffffffffffff", static_cast<uint64_t>(0xffff'ffff'ffff'ffff));
}

}}