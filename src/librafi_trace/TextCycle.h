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
#include <fstream>
#include <vector>
#include <rafi/trace.h>

namespace rafi { namespace trace {

class TextCycle : ICycle
{
public:
    static std::unique_ptr<TextCycle> Parse(std::ifstream& input);

    TextCycle(XLEN xlen);
    virtual ~TextCycle();

    virtual XLEN GetXLEN() = 0;

    virtual bool IsPcExist() = 0;
    virtual bool IsIntRegExist() = 0;
    virtual bool IsFpRegExist() = 0;

    virtual uint64_t GetPc(bool isPhysical) = 0;
    virtual uint64_t GetIntReg(int index) = 0;
    virtual uint64_t GetFpReg(int index) = 0;

private:
    void ParsePc(std::ifstream& input);
    void ParseIntReg(std::ifstream& input);
    void ParseFpReg(std::ifstream& input);

    XLEN m_XLEN;

    bool m_PcExist{ false };
    bool m_IntRegExist{ false };
    bool m_FpRegExist{ false };

    uint64_t m_VirtualPc{ 0 };
    uint64_t m_PhysicalPc{ 0 };

    uint64_t m_IntRegs[IntRegCount];
    uint64_t m_FpRegs[IntRegCount];
};

}}
