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

    virtual void CopyOpEvent(NodeOpEvent* pOutEvent, int index) const override;
    virtual void CopyMemoryEvent(NodeMemoryEvent* pOutEvent, int index) const override;
    virtual void CopyTrapEvent(NodeTrapEvent* pOutEvent, int index) const override;
    virtual void CopyIo(NodeIo* pOutState) const override;
    virtual void CopyNote(std::string* pOutNote) const override;

private:
    int GetNodeCount(NodeType nodeType) const;
    int64_t GetNodeSize(NodeType nodeType, int index) const;
    const void* GetNode(NodeType nodeType, int index) const;

    const BasicInfoNode* GetBasicInfoNode() const;
    const FpRegNode* GetFpRegNode() const;
    const IntReg32Node* GetIntReg32Node() const;
    const IntReg64Node* GetIntReg64Node() const;
    const Pc32Node* GetPc32Node() const;
    const Pc64Node* GetPc64Node() const;
    const Trap32Node* GetTrap32Node() const;
    const Trap64Node* GetTrap64Node() const;
    const MemoryAccessNode* GetMemoryAccessNode(int index) const;
    const IoNode* GetIoNode() const;
    const void* GetMemoryNode() const;

    void CheckNodeSizeEqualTo(NodeType nodeType, int index, size_t size) const;
    void CheckNodeSizeGreaterThan(NodeType nodeType, int index, size_t size) const;

    const CycleHeader* GetPointerToHeader() const;
    const CycleMetaNode* GetPointerToMeta(uint32_t index) const;
    const CycleMetaNode* GetPointerToMeta(NodeType nodeType, int index) const;

    const void* GetPointerToNode(NodeType nodeType, int index) const;

    const void* m_pBuffer;
    int64_t m_BufferSize;
};

}}
