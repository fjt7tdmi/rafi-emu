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

#include <stdint.h>

#include <string>

#include <rafi/common.h>
#include <rafi/trace/CycleTypes.h>

namespace rafi { namespace trace {

class ICycle
{
public:
    virtual ~ICycle(){}

    virtual XLEN GetXLEN() const = 0;

    virtual bool IsPcExist() const = 0;
    virtual bool IsIntRegExist() const = 0;
    virtual bool IsFpRegExist() const = 0;

    virtual int GetMemoryAccessCount() const = 0;

    virtual uint64_t GetPc(bool isPhysical) const = 0;
    virtual uint64_t GetIntReg(int index) const = 0;
    virtual uint64_t GetFpReg(int index) const = 0;

    virtual void CopyMemoryAccess(MemoryAccessNode* pOutNode, int index) const = 0;
    virtual const char* GetNote() const = 0;
};

}}
