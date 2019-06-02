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

#include <cstring>

#include <rafi/trace.h>

#include "GdbCycle.h"

namespace rafi { namespace trace {

namespace {
    const char* g_IntRegNames[IntRegCount] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
    };
}

std::unique_ptr<GdbCycle> GdbCycle::Parse(std::basic_istream<char>* pInput)
{
    auto p = std::make_unique<GdbCycle>();

    for (;;)
    {
        std::string head;
        *pInput >> head;

        if (head == "BREAK")
        {
            break;
        }
        else if (head == "pc")
        {
            std::string tmp;
            *pInput >> std::hex >> p->m_Pc >> tmp;
        }
        else if (head == "priv")
        {
            std::string tmp;
            *pInput >> tmp;
        }
        else
        {
            bool parsed = false;

            for (int i = 0; i < IntRegCount; i++)
            {
                if (head == g_IntRegNames[i])
                {
                    std::string tmp;
                    *pInput >> std::hex >> p->m_IntRegs[i] >> tmp;

                    parsed = true;
                    break;
                }
            }

            if (!parsed)
            {
                throw TraceException("Trace text parse error: unknown literal.");
            }
        }
    }

    return p;
}

GdbCycle::GdbCycle()
{
    std::memset(m_IntRegs, 0, sizeof(m_IntRegs));
}

GdbCycle::~GdbCycle()
{
}

XLEN GdbCycle::GetXLEN() const
{
    return XLEN::XLEN64;
}

bool GdbCycle::IsPcExist() const
{
    return true;
}

bool GdbCycle::IsIntRegExist() const
{
    return true;
}

bool GdbCycle::IsFpRegExist() const
{
    return false;
}

int GdbCycle::GetMemoryAccessCount() const
{
    return 0;
}

uint64_t GdbCycle::GetPc(bool isPhysical) const
{
    return isPhysical ? 0 : m_Pc;
}

uint64_t GdbCycle::GetIntReg(int index) const
{
    if (!(0 <= index && index < IntRegCount))
    {
        throw TraceException("Specified index is out of range");
    }

    return m_IntRegs[index];
}

uint64_t GdbCycle::GetFpReg(int index) const
{
    (void)index;
    return 0;
}

void GdbCycle::CopyMemoryAccess(MemoryAccessNode* pOutNode, int index) const
{
    (void)pOutNode;
    (void)index;
    RAFI_NOT_IMPLEMENTED();
}

}}
