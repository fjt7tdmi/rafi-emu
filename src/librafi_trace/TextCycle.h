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

#pragma once

#include <cstdint>
#include <string>
#include <istream>
#include <vector>
#include <rafi/trace.h>

namespace rafi { namespace trace {

class TextCycle : public ICycle
{
public:
    static std::unique_ptr<TextCycle> Parse(std::basic_istream<char>* pInput, XLEN xlen);

    TextCycle(XLEN xlen);
    virtual ~TextCycle();

    virtual XLEN GetXLEN() const;

    virtual bool IsPcExist() const;
    virtual bool IsIntRegExist() const;
    virtual bool IsFpRegExist() const;
    virtual bool IsIoStateExist() const;
    virtual bool IsNoteExist() const;

    virtual int GetOpEventCount() const override;
    virtual int GetMemoryEventCount() const;
    virtual int GetTrapEventCount() const;

    virtual uint64_t GetPc(bool isPhysical) const;
    virtual uint64_t GetIntReg(int index) const;
    virtual uint64_t GetFpReg(int index) const;

    virtual void CopyIoState(IoState* pOutState) const;
    virtual void CopyNote(std::string* pOutNote) const;
    virtual void CopyOpEvent(OpEvent* pOutEvent, int index) const override;
    virtual void CopyMemoryEvent(MemoryEvent* pOutEvent, int index) const;
    virtual void CopyTrapEvent(TrapEvent* pOutEvent, int index) const;

private:
    void ParsePc(std::basic_istream<char>* pInput);
    void ParseIntReg(std::basic_istream<char>* pInput);
    void ParseFpReg(std::basic_istream<char>* pInput);
    void ParseNote(std::basic_istream<char>* pInput);

    XLEN m_XLEN;

    bool m_PcExist{ false };
    bool m_IntRegExist{ false };
    bool m_FpRegExist{ false };

    uint64_t m_VirtualPc{ 0 };
    uint64_t m_PhysicalPc{ 0 };

    uint64_t m_IntRegs[IntRegCount];
    uint64_t m_FpRegs[IntRegCount];

    std::string m_Note;
};

}}
