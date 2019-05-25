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

#include "TextTrace.h"

namespace rafi { namespace trace {

TextTrace::TextTrace(std::basic_istream<char>* pInput)
    : m_pInput(pInput)
{
    // Read XLEN node
    std::string s;
    *m_pInput >> s;

    if (s != "XLEN")
    {
        throw TraceException("First literal is not 'XLEN'");
    }

    *m_pInput >> s;

    if (s == "32")
    {
        m_XLEN = XLEN::XLEN32;
    }
    else if (s == "64")
    {
        m_XLEN = XLEN::XLEN64;
    }
    else
    {
        throw TraceException("Parameter of XLEN is invalid.");
    }

    // Read first cycle
    UpdateTextCycle();
}

TextTrace::~TextTrace()
{
}

const ICycle* TextTrace::GetCycle() const
{
    return m_pTextCycle.get();
}

bool TextTrace::IsEnd() const
{
    return !m_pTextCycle;
}

void TextTrace::Next()
{
    if (IsEnd())
    {
        throw TraceException("TextTrace reached the end of input.");
    }

    UpdateTextCycle();
}

void TextTrace::UpdateTextCycle()
{
    try
    {
        m_pTextCycle = TextCycle::Parse(m_pInput, m_XLEN);
    }
    catch (const TraceException&)
    {
        m_pTextCycle = nullptr;
    }
}

}}
