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

#include <cstring>
#include <rafi/trace.h>

#include "BinaryCycle.h"

namespace rafi { namespace trace {

BinaryCycle::BinaryCycle(const void* buffer, int64_t bufferSize)
    : m_pBuffer(buffer)
    , m_BufferSize(bufferSize)
{
}

BinaryCycle::~BinaryCycle()
{
}

XLEN BinaryCycle::GetXLEN() const
{
    if (GetNodeCount(NodeType::Pc32) > 0)
    {
        return XLEN::XLEN32;
    }
    else if (GetNodeCount(NodeType::Pc64) > 0)
    {
        return XLEN::XLEN64;
    }
    else
    {
        throw TraceException("GetXLEN() failed.");
    }
}

bool BinaryCycle::IsPcExist() const
{
    const auto nodeType = (GetXLEN() == XLEN::XLEN32) ? NodeType::Pc32 : NodeType::Pc64;

    return GetNodeCount(nodeType) > 0;
}

bool BinaryCycle::IsIntRegExist() const
{
    const auto nodeType = (GetXLEN() == XLEN::XLEN32) ? NodeType::IntReg32 : NodeType::IntReg64;

    return GetNodeCount(nodeType) > 0;
}

bool BinaryCycle::IsFpRegExist() const
{
    return GetNodeCount(NodeType::FpReg) > 0;
}

bool BinaryCycle::IsIoStateExist() const
{
    return GetNodeCount(NodeType::Io) > 0;
}

bool BinaryCycle::IsNoteExist() const
{
    return false;
}

int BinaryCycle::GetOpEventCount() const
{
    return 1;
}

int BinaryCycle::GetMemoryEventCount() const
{
    return GetNodeCount(trace::NodeType::MemoryAccess);
}

int BinaryCycle::GetTrapEventCount() const
{
    return GetNodeCount(trace::NodeType::Trap32) + GetNodeCount(trace::NodeType::Trap64);
}

uint64_t BinaryCycle::GetPc(bool isPhysical) const
{
    if (GetXLEN() == XLEN::XLEN32)
    {
        auto node = GetPc32Node();

        return isPhysical ? node->physicalPc : GetPc32Node()->virtualPc;
    }
    else
    {
        auto node = GetPc64Node();

        return isPhysical ? node->physicalPc : GetPc64Node()->virtualPc;
    }
}

uint64_t BinaryCycle::GetIntReg(int index) const
{
    if (!(0 <= index && index < IntRegCount))
    {
        throw TraceException("Specified index is out-of-range.");
    }

    if (GetXLEN() == XLEN::XLEN32)
    {
        return GetIntReg32Node()->regs[index];
    }
    else
    {
        return GetIntReg64Node()->regs[index];
    }
}

uint64_t BinaryCycle::GetFpReg(int index) const
{
    if (!(0 <= index && index < FpRegCount))
    {
        throw TraceException("Specified index is out-of-range.");
    }

    return GetFpRegNode()->regs[index].u64.value;
}

void BinaryCycle::CopyIo(NodeIo* pOutState) const
{
    pOutState->hostIo = GetIoNode()->hostIoValue;
    pOutState->reserved = 0;
}

void BinaryCycle::CopyNote(std::string* pOutNote) const
{
    *pOutNote = "(null)";
}

void BinaryCycle::CopyOpEvent(NodeOpEvent* pOutEvent, int index) const
{
    (void)index;
    pOutEvent->insn = GetBasicInfoNode()->insn;
    pOutEvent->priv = GetBasicInfoNode()->privilegeLevel;
}

void BinaryCycle::CopyMemoryEvent(NodeMemoryEvent* pOutEvent, int index) const
{
    std::memcpy(pOutEvent, GetMemoryAccessNode(index), sizeof(MemoryAccessNode));
}

void BinaryCycle::CopyTrapEvent(NodeTrapEvent* pOutEvent, int index) const
{
    (void)index;

    if (GetXLEN() == XLEN::XLEN32)
    {
        auto pNode = GetTrap32Node();

        pOutEvent->cause = pNode->cause;
        pOutEvent->from = pNode->from;
        pOutEvent->to = pNode->to;
        pOutEvent->trapType = pNode->trapType;
        pOutEvent->trapValue = pNode->trapValue;
    }
    else
    {
        auto pNode = GetTrap64Node();

        pOutEvent->cause = pNode->cause;
        pOutEvent->from = pNode->from;
        pOutEvent->to = pNode->to;
        pOutEvent->trapType = pNode->trapType;
        pOutEvent->trapValue = pNode->trapValue;
    }
}

int BinaryCycle::GetNodeCount(NodeType nodeType) const
{
    int count = 0;

    for (auto i = 0u; i < GetPointerToHeader()->metaCount; i++)
    {
        auto pMeta = GetPointerToMeta(i);

        if (pMeta->nodeType == nodeType)
        {
            count++;
        }
    }

    return count;
}

const void* BinaryCycle::GetNode(NodeType nodeType, int index) const
{
    const auto pNode = GetPointerToNode(nodeType, index);

    if (pNode == nullptr)
    {
        throw TraceException("Cannot find node.");
    }

    return pNode;
}

int64_t BinaryCycle::GetNodeSize(NodeType nodeType, int index) const
{
    const auto pMeta = GetPointerToMeta(nodeType, index);

    if (pMeta == nullptr)
    {
        throw TraceException("Cannot find node.");
    }

    return pMeta->size;
}

const BasicInfoNode* BinaryCycle::GetBasicInfoNode() const
{
    CheckNodeSizeEqualTo(NodeType::BasicInfo, 0, sizeof(BasicInfoNode));
    return reinterpret_cast<const BasicInfoNode*>(GetNode(NodeType::BasicInfo, 0));
}

const FpRegNode* BinaryCycle::GetFpRegNode() const
{
    CheckNodeSizeEqualTo(NodeType::FpReg, 0, sizeof(FpRegNode));
    return reinterpret_cast<const FpRegNode*>(GetNode(NodeType::FpReg, 0));
}

const IntReg32Node* BinaryCycle::GetIntReg32Node() const
{
    CheckNodeSizeEqualTo(NodeType::IntReg32, 0, sizeof(IntReg32Node));
    return reinterpret_cast<const IntReg32Node*>(GetNode(NodeType::IntReg32, 0));
}

const IntReg64Node* BinaryCycle::GetIntReg64Node() const
{
    CheckNodeSizeEqualTo(NodeType::IntReg64, 0, sizeof(IntReg64Node));
    return reinterpret_cast<const IntReg64Node*>(GetNode(NodeType::IntReg64, 0));
}

const Pc32Node* BinaryCycle::GetPc32Node() const
{
    CheckNodeSizeEqualTo(NodeType::Pc32, 0, sizeof(Pc32Node));
    return reinterpret_cast<const Pc32Node*>(GetNode(NodeType::Pc32, 0));
}

const Pc64Node* BinaryCycle::GetPc64Node() const
{
    CheckNodeSizeEqualTo(NodeType::Pc64, 0, sizeof(Pc64Node));
    return reinterpret_cast<const Pc64Node*>(GetNode(NodeType::Pc64, 0));
}

const Trap32Node* BinaryCycle::GetTrap32Node() const
{
    CheckNodeSizeEqualTo(NodeType::Trap32, 0, sizeof(Trap32Node));
    return reinterpret_cast<const Trap32Node*>(GetNode(NodeType::Trap32, 0));
}

const Trap64Node* BinaryCycle::GetTrap64Node() const
{
    CheckNodeSizeEqualTo(NodeType::Trap64, 0, sizeof(Trap64Node));
    return reinterpret_cast<const Trap64Node*>(GetNode(NodeType::Trap64, 0));
}

const MemoryAccessNode* BinaryCycle::GetMemoryAccessNode(int index) const
{
    CheckNodeSizeEqualTo(NodeType::MemoryAccess, index, sizeof(MemoryAccessNode));
    return reinterpret_cast<const MemoryAccessNode*>(GetNode(NodeType::MemoryAccess, index));
}

const IoNode* BinaryCycle::GetIoNode() const
{
    CheckNodeSizeEqualTo(NodeType::Io, 0, sizeof(IoNode));
    return reinterpret_cast<const IoNode*>(GetNode(NodeType::Io, 0));
}

void BinaryCycle::CheckNodeSizeEqualTo(NodeType nodeType, int index, size_t size) const
{
    const auto nodeSize = GetNodeSize(nodeType, index);

    if (nodeSize < 0 || static_cast<size_t>(nodeSize) != size)
    {
        throw TraceException("node size is incorrect.");
    }
}

void BinaryCycle::CheckNodeSizeGreaterThan(NodeType nodeType, int index, size_t size) const
{
    const auto nodeSize = GetNodeSize(nodeType, index);

    if (nodeSize < 0 || static_cast<size_t>(nodeSize) <= size)
    {
        throw TraceException("node size is incorrect.");
    }
}

const CycleHeader* BinaryCycle::GetPointerToHeader() const
{
    return reinterpret_cast<const CycleHeader*>(m_pBuffer);
}

const CycleMetaNode* BinaryCycle::GetPointerToMeta(uint32_t index) const
{
    assert(0 <= index);
    assert(index < GetPointerToHeader()->metaCount);

    const auto metaNodes = reinterpret_cast<const CycleMetaNode*>(GetPointerToHeader() + 1);

    return &metaNodes[index];
}

const CycleMetaNode* BinaryCycle::GetPointerToMeta(NodeType nodeType, int index) const
{
    const auto metaCount = GetPointerToHeader()->metaCount;

    int matched = 0;

    for (auto i = 0u; i < metaCount; i++)
    {
        auto pMeta = GetPointerToMeta(i);

        if (pMeta->nodeType == nodeType)
        {
            if (index == matched)
            {
                return pMeta;
            }

            matched++;
        }
    }

    return nullptr;
}

const void* BinaryCycle::GetPointerToNode(NodeType nodeType, int index) const
{
    const auto pMeta = GetPointerToMeta(nodeType, index);

    if (pMeta == nullptr)
    {
        return nullptr;
    }

    return reinterpret_cast<const uint8_t*>(m_pBuffer) + pMeta->offset;
}

}}
