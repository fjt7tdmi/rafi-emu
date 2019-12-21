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

std::unique_ptr<TextCycle> TextCycle::Parse(std::basic_istream<char>* pInput, XLEN xlen)
{
    auto p = std::make_unique<TextCycle>(xlen);

    for (;;)
    {
        std::string s;
        *pInput >> s;

        if (s == "BREAK")
        {
            break;
        }
        else if (s == "BASIC")
        {
            p->ParseBasic(pInput);
        }
        else if (s == "INT")
        {
            p->ParseIntReg(pInput);
        }
        else if (s == "FP")
        {
            p->ParseIntReg(pInput);
        }
        else
        {
            throw TraceException("Trace text parse error: unknown literal.");
        }
    }

    return p;
}

TextCycle::TextCycle(XLEN xlen)
    : m_XLEN(xlen)
{
}

TextCycle::~TextCycle()
{
}

uint32_t TextCycle::GetCycle() const
{
    if (!m_BasicExist)
    {
        throw TraceException("NodeBasic is not exist.");
    }

    return m_CycleCount;
}

XLEN TextCycle::GetXLEN() const
{
    if (!m_BasicExist)
    {
        throw TraceException("NodeBasic is not exist.");
    }

    return m_XLEN;
}

uint64_t TextCycle::GetPc() const
{
    if (!m_BasicExist)
    {
        throw TraceException("NodeBasic is not exist.");
    }

    return m_Pc;
}

bool TextCycle::IsIntRegExist() const
{
    return m_IntRegExist;
}

bool TextCycle::IsFpRegExist() const
{
    return m_FpRegExist;
}

bool TextCycle::IsIoExist() const
{
    return false;
}

size_t TextCycle::GetOpEventCount() const
{
    return 0;
}

size_t TextCycle::GetMemoryEventCount() const
{
    return 0;
}

size_t TextCycle::GetTrapEventCount() const
{
    return 0;
}

uint64_t TextCycle::GetIntReg(size_t index) const
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

uint64_t TextCycle::GetFpReg(size_t index) const
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

void TextCycle::CopyIo(NodeIo* pOutNode) const
{
    (void)pOutNode;
    RAFI_NOT_IMPLEMENTED;
}

void TextCycle::CopyOpEvent(NodeOpEvent* pOutNode, size_t index) const
{
    (void)pOutNode;
    (void)index;
    RAFI_NOT_IMPLEMENTED;
}

void TextCycle::CopyMemoryEvent(NodeMemoryEvent* pOutNode, size_t index) const
{
    (void)pOutNode;
    (void)index;
    RAFI_NOT_IMPLEMENTED;
}

void TextCycle::CopyTrapEvent(NodeTrapEvent* pOutNode, size_t index) const
{
    (void)pOutNode;
    (void)index;
    RAFI_NOT_IMPLEMENTED;
}

void TextCycle::ParseBasic(std::basic_istream<char>* pInput)
{
    uint32_t xlen;
    *pInput >> std::hex >> m_CycleCount >> xlen >> m_Pc;

    m_XLEN = static_cast<XLEN>(xlen);
    m_BasicExist = true;
}

void TextCycle::ParseIntReg(std::basic_istream<char>* pInput)
{
    for (int i = 0; i < IntRegCount; i++)
    {
        *pInput >> std::hex >> m_IntRegs[i];
    }

    m_IntRegExist = true;
}

void TextCycle::ParseFpReg(std::basic_istream<char>* pInput)
{
    for (int i = 0; i < IntRegCount; i++)
    {
        *pInput >> std::hex >> m_FpRegs[i];
    }

    m_FpRegExist = true;
}

}}
