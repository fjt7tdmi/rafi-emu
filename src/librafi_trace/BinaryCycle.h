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

#include <rafi/trace.h>

#include "CycleViewImpl.h"

namespace rafi { namespace trace {

class BinaryCycle : public ICycle
{
public:
    BinaryCycle(const void* buffer, int64_t bufferSize);
    virtual ~BinaryCycle() override;

    virtual XLEN GetXLEN() const override;

    virtual bool IsPcExist() const override;
    virtual bool IsIntRegExist() const override;
    virtual bool IsFpRegExist() const override;
    virtual bool IsIoStateExist() const;
    virtual bool IsNoteExist() const;

    virtual int GetOpEventCount() const override;
    virtual int GetMemoryEventCount() const override;
    virtual int GetTrapEventCount() const override;

    virtual uint64_t GetPc(bool isPhysical) const override;
    virtual uint64_t GetIntReg(int index) const override;
    virtual uint64_t GetFpReg(int index) const override;

    virtual void CopyOpEvent(OpEvent* pOutEvent, int index) const override;
    virtual void CopyMemoryEvent(MemoryEvent* pOutEvent, int index) const override;
    virtual void CopyTrapEvent(TrapEvent* pOutEvent, int index) const override;
    virtual void CopyIoState(IoState* pOutState) const override;
    virtual void CopyNote(std::string* pOutNote) const override;

private:
    CycleViewImpl m_Impl;
};

}}
