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

std::unique_ptr<GdbCycle> GdbCycle::Parse(std::basic_istream<char>* pInput, uint32_t cycle)
{
    auto p = std::make_unique<GdbCycle>();
    
    p->m_CycleCount = cycle;
    
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

uint32_t GdbCycle::GetCycle() const
{
    return m_CycleCount;
}

XLEN GdbCycle::GetXLEN() const
{
    return XLEN::XLEN64;
}

uint64_t GdbCycle::GetPc() const
{
    return m_Pc;
}

bool GdbCycle::IsIntRegExist() const
{
    return true;
}

bool GdbCycle::IsFpRegExist() const
{
    return false;
}

bool GdbCycle::IsIoExist() const
{
    return false;
}

size_t GdbCycle::GetOpEventCount() const
{
    return 0;
}

size_t GdbCycle::GetMemoryEventCount() const
{
    return 0;
}

size_t GdbCycle::GetTrapEventCount() const
{
    return 0;
}

uint64_t GdbCycle::GetIntReg(size_t index) const
{
    if (!(0 <= index && index < IntRegCount))
    {
        throw TraceException("Specified index is out of range");
    }

    return m_IntRegs[index];
}

uint64_t GdbCycle::GetFpReg(size_t index) const
{
    (void)index;
    return 0;
}

void GdbCycle::CopyIo(NodeIo* pOutState) const
{
    (void)pOutState;
    RAFI_NOT_IMPLEMENTED();
}

void GdbCycle::CopyOpEvent(NodeOpEvent* pOutEvent, size_t index) const
{
    (void)pOutEvent;
    (void)index;
    RAFI_NOT_IMPLEMENTED();
}

void GdbCycle::CopyMemoryEvent(NodeMemoryEvent* pOutEvent, size_t index) const
{
    (void)pOutEvent;
    (void)index;
    RAFI_NOT_IMPLEMENTED();
}

void GdbCycle::CopyTrapEvent(NodeTrapEvent* pOutEvent, size_t index) const
{
    (void)pOutEvent;
    (void)index;
    RAFI_NOT_IMPLEMENTED();
}

}}
