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

#include "../../src/librafi_trace/TextTrace.h"

namespace rafi { namespace trace {

TEST(TextTrace, Basic)
{
    const char* text =
        "XLEN 32\n"
        "PC 80000000 1000\n"
        "INT\n"
        "00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n"
        "10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f\n"
        "BREAK\n"
        "PC 80000004 1004\n"
        "BREAK\n";

    std::stringstream ss(text);

    auto pTrace = std::make_unique<TextTrace>(&ss);

    ASSERT_FALSE(pTrace->IsEnd());

    // cycle 0
    {
        const auto pCycle = pTrace->GetCycle();

        ASSERT_EQ(XLEN::XLEN32, pCycle->GetXLEN());

        ASSERT_TRUE(pCycle->IsPcExist());
        ASSERT_TRUE(pCycle->IsIntRegExist());
        ASSERT_FALSE(pCycle->IsFpRegExist());

        ASSERT_EQ(0x80000000, pCycle->GetPc(false));
        ASSERT_EQ(0x1000, pCycle->GetPc(true));

        for (int i = 0; i < IntRegCount; i++)
        {
            ASSERT_EQ(i, pCycle->GetIntReg(i));
        }
    }

    pTrace->Next();
    ASSERT_FALSE(pTrace->IsEnd());

    // cycle 1
    {
        const auto pCycle = pTrace->GetCycle();

        ASSERT_EQ(XLEN::XLEN32, pCycle->GetXLEN());

        ASSERT_TRUE(pCycle->IsPcExist());
        ASSERT_FALSE(pCycle->IsIntRegExist());
        ASSERT_FALSE(pCycle->IsFpRegExist());

        ASSERT_EQ(0x80000004, pCycle->GetPc(false));
        ASSERT_EQ(0x1004, pCycle->GetPc(true));
    }

    pTrace->Next();
    ASSERT_TRUE(pTrace->IsEnd());
}

}}