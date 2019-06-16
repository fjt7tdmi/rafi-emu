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

#include <fstream>

#include <rafi/trace.h>

#include "GdbTrace.h"

namespace rafi { namespace trace {

GdbTrace::GdbTrace(std::basic_istream<char>* pInput)
    : m_pInput(pInput)
{
    Update();
}

GdbTrace::~GdbTrace()
{
}

const ICycle* GdbTrace::GetCycle() const
{
    return m_pGdbCycle.get();
}

bool GdbTrace::IsEnd() const
{
    return !m_pGdbCycle;
}

void GdbTrace::Next()
{
    if (IsEnd())
    {
        throw TraceException("GdbTrace reached the end of input.");
    }

    Update();
}

void GdbTrace::Update()
{
    try
    {
        m_pGdbCycle = GdbCycle::Parse(m_pInput, m_CycleCount++);
    }
    catch (const TraceException&)
    {
        m_pGdbCycle = nullptr;
    }
}

}}
