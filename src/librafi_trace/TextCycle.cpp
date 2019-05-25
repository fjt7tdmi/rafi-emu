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

#include "TextCycle.h"

namespace rafi { namespace trace {

std::unique_ptr<TextCycle> TextCycle::Parse(std::ifstream& input)
{
    auto p = std::make_unique<TextCycle>();

    std::string s;

    for (;;)
    {
        input >> s;

        if (s == "BREAK")
        {
            break;
        }
        else if (s == "PC")
        {
            p->ParsePc(input);
        }
        else if (s == "INT")
        {
            p->ParseIntReg(input);
        }
        else if (s == "FP")
        {
            p->ParseIntReg(input);
        }
        else
        {
            throw TraceException("Trace text parse error: unknown literal.");
        }
    }
}

TextCycle::TextCycle(XLEN xlen)
    : m_XLEN(xlen)
{
}

TextCycle::~TextCycle()
{
}

XLEN TextCycle::GetXLEN()
{
    return m_XLEN;
}

bool TextCycle::IsPcExist()
{
    return m_PcExist;
}

bool TextCycle::IsIntRegExist()
{
    return m_IntRegExist;
}

bool TextCycle::IsFpRegExist()
{
    return m_FpRegExist;
}

uint64_t TextCycle::GetPc(bool isPhysical)
{
    if (!m_PcExist)
    {
        throw TraceException("PC value is not exist.");
    }
    
    return isPhysical ? m_PhysicalPc : m_VirtualPc;
}

uint64_t TextCycle::GetIntReg(int index)
{
    if (!m_IntRegExist)
    {
        throw TraceException("Integer register values are not exist.");
    }

    if (!(0 <= index && index < IntRegCount))
    {
        throw TraceException("Specified index is out of range");
    }
    
    return m_IntRegs[index];
}

uint64_t TextCycle::GetFpReg(int index)
{
    if (!m_IntRegExist)
    {
        throw TraceException("Integer register values are not exist.");
    }

    if (!(0 <= index && index < IntRegCount))
    {
        throw TraceException("Specified index is out of range");
    }

    return m_FpRegs[index];
}

void TextCycle::ParsePc(std::ifstream& input)
{
    input >> std::hex >> m_VirtualPc >> m_PhysicalPc;

    m_PcExist = true;
}

void TextCycle::ParseIntReg(std::ifstream& input)
{
    for (int i = 0; i < IntRegCount; i++)
    {
        input >> std::hex >> m_IntRegs[i];
    }

    m_IntRegExist = true;
}

void TextCycle::ParseFpReg(std::ifstream& input)
{
    for (int i = 0; i < IntRegCount; i++)
    {
        input >> std::hex >> m_FpRegs[i];
    }

    m_FpRegExist = true;
}

}}
