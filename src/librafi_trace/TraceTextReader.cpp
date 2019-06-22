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

#include <rafi/trace.h>

#include "TextTrace.h"

namespace rafi { namespace trace {

TraceTextReader::TraceTextReader(const char* path)
{
    m_pInput = new std::ifstream(path, std::ios::in);
    m_pTrace = new TextTrace(m_pInput);
}

TraceTextReader::~TraceTextReader()
{
    if (m_pTrace != nullptr)
    {
        delete m_pTrace;
    }
    if (m_pInput != nullptr)
    {
        delete m_pInput;
    }
}

const ICycle* TraceTextReader::GetCycle() const
{
    return m_pTrace->GetCycle();
}

bool TraceTextReader::IsEnd() const
{
    return m_pTrace->IsEnd();
}

void TraceTextReader::Next()
{
    m_pTrace->Next();
}

void TraceTextReader::Next(uint32_t cycle)
{
    for (uint32_t i = 0; i < cycle; i++)
    {
        Next();
    }
}

}}
