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
        else if (s == "PC")
        {
            p->ParsePc(pInput);
        }
        else if (s == "INT")
        {
            p->ParseIntReg(pInput);
        }
        else if (s == "FP")
        {
            p->ParseIntReg(pInput);
        }
        else if (s == "NOTE")
        {
            p->ParseNote(pInput);
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

XLEN TextCycle::GetXLEN() const
{
    return m_XLEN;
}

bool TextCycle::IsPcExist() const
{
    return m_PcExist;
}

bool TextCycle::IsIntRegExist() const
{
    return m_IntRegExist;
}

bool TextCycle::IsFpRegExist() const
{
    return m_FpRegExist;
}

bool TextCycle::IsIoStateExist() const
{
    return false;
}

bool TextCycle::IsNoteExist() const
{
    return !m_Note.empty();
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

uint64_t TextCycle::GetPc(bool isPhysical) const
{
    if (!m_PcExist)
    {
        throw TraceException("PC value is not exist.");
    }

    return isPhysical ? m_PhysicalPc : m_VirtualPc;
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

void TextCycle::CopyIo(NodeIo* pOutState) const
{
    (void)pOutState;
    RAFI_NOT_IMPLEMENTED();
}

void TextCycle::CopyNote(std::string* pOutNote) const
{
    *pOutNote = m_Note;
}

void TextCycle::CopyOpEvent(NodeOpEvent* pOutEvent, size_t index) const
{
    (void)pOutEvent;
    (void)index;
    RAFI_NOT_IMPLEMENTED();
}

void TextCycle::CopyMemoryEvent(NodeMemoryEvent* pOutEvent, size_t index) const
{
    (void)pOutEvent;
    (void)index;
    RAFI_NOT_IMPLEMENTED();
}

void TextCycle::CopyTrapEvent(NodeTrapEvent* pOutEvent, size_t index) const
{
    (void)pOutEvent;
    (void)index;
    RAFI_NOT_IMPLEMENTED();
}

void TextCycle::ParsePc(std::basic_istream<char>* pInput)
{
    *pInput >> std::hex >> m_VirtualPc >> m_PhysicalPc;

    m_PcExist = true;
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

void TextCycle::ParseNote(std::basic_istream<char>* pInput)
{
    *pInput >> m_Note;
}

}}
