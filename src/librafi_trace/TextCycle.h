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

    virtual uint32_t GetCycle() const override;
    virtual XLEN GetXLEN() const override;
    virtual uint64_t GetPc() const override;

    virtual bool IsIntRegExist() const override;
    virtual bool IsFpRegExist() const override;
    virtual bool IsIoExist() const override;

    virtual size_t GetOpEventCount() const override;
    virtual size_t GetMemoryEventCount() const override;
    virtual size_t GetTrapEventCount() const override;

    virtual uint64_t GetIntReg(size_t index) const override;
    virtual uint64_t GetFpReg(size_t index) const override;

    virtual void CopyIo(NodeIo* pOutState) const override;
    virtual void CopyOpEvent(NodeOpEvent* pOutEvent, size_t index) const override;
    virtual void CopyMemoryEvent(NodeMemoryEvent* pOutEvent, size_t index) const override;
    virtual void CopyTrapEvent(NodeTrapEvent* pOutEvent, size_t index) const override;

private:
    void ParsePc(std::basic_istream<char>* pInput);
    void ParseIntReg(std::basic_istream<char>* pInput);
    void ParseFpReg(std::basic_istream<char>* pInput);

    XLEN m_XLEN;

    bool m_PcExist{ false };
    bool m_IntRegExist{ false };
    bool m_FpRegExist{ false };

    uint64_t m_Pc{ 0 };

    uint64_t m_IntRegs[IntRegCount];
    uint64_t m_FpRegs[IntRegCount];
};

}}
