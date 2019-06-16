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

    virtual uint32_t GetCycle() const = 0;
    virtual XLEN GetXLEN() const = 0;
    virtual uint64_t GetPc() const = 0;

    virtual bool IsIntRegExist() const = 0;
    virtual bool IsFpRegExist() const = 0;
    virtual bool IsIoExist() const = 0;

    virtual size_t GetOpEventCount() const = 0;
    virtual size_t GetMemoryEventCount() const = 0;
    virtual size_t GetTrapEventCount() const = 0;

    virtual uint64_t GetIntReg(size_t index) const = 0;
    virtual uint64_t GetFpReg(size_t index) const = 0;

    virtual void CopyIo(NodeIo* pOutState) const = 0;
    virtual void CopyOpEvent(NodeOpEvent* pOutEvent, size_t index) const = 0;
    virtual void CopyMemoryEvent(NodeMemoryEvent* pOutEvent, size_t index) const = 0;
    virtual void CopyTrapEvent(NodeTrapEvent* pOutEvent, size_t index) const = 0;
};

}}
