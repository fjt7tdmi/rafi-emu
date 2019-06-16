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

#include <memory>
#include <vector>

#include <rafi/trace.h>

namespace rafi { namespace trace {

class BinaryCycle : public ICycle
{
public:
    static std::unique_ptr<BinaryCycle> Parse(const void* buffer, size_t bufferSize);

    BinaryCycle();
    virtual ~BinaryCycle() override;

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

    size_t GetSize() const;

private:
    size_t ParseNode(const void* buffer, size_t bufferSize);

    const void* m_pBuffer{ nullptr };
    size_t m_BufferSize{ 0 };
    size_t m_Size{ 0 };

    const NodeBasic* m_pNodeBasic{ nullptr };
    const NodeIntReg32* m_pNodeIntReg32{ nullptr };
    const NodeIntReg64* m_pNodeIntReg64{ nullptr };
    const NodeFpReg* m_pNodeFpReg{ nullptr };
    const NodeIo* m_pNodeIo{ nullptr };

    std::vector<const NodeOpEvent*> m_OpEvents;
    std::vector<const NodeMemoryEvent*> m_MemoryEvents;
    std::vector<const NodeTrapEvent*> m_TrapEvents;

    bool m_Break{ false };
};

}}
