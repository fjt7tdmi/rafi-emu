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

class GdbCycle : public ICycle
{
public:
    static std::unique_ptr<GdbCycle> Parse(std::basic_istream<char>* pInput);

    GdbCycle();
    virtual ~GdbCycle();

    virtual XLEN GetXLEN() const;

    virtual bool IsPcExist() const;
    virtual bool IsIntRegExist() const;
    virtual bool IsFpRegExist() const;

    virtual int GetMemoryAccessCount() const;

    virtual uint64_t GetPc(bool isPhysical) const;
    virtual uint64_t GetIntReg(int index) const;
    virtual uint64_t GetFpReg(int index) const;

    virtual void CopyMemoryAccess(MemoryAccessNode* pOutNode, int index) const;

    virtual const char* GetNote() const
    {
        return "";
    }

private:
    uint64_t m_Pc{ 0 };
    uint64_t m_IntRegs[IntRegCount];
};

}}
