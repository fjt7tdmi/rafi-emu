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

#include <cassert>
#include <cstddef>
#include <cstdint>

#include <rafi/trace.h>

#include "CycleViewImpl.h"

namespace rafi { namespace trace {

CycleViewImpl::CycleViewImpl(const void* buffer, int64_t bufferSize)
{
    m_pData = buffer;
    m_BufferSize = bufferSize;
}

const void* CycleViewImpl::GetNode(NodeType nodeType, int index) const
{
    const auto pNode = GetPointerToNode(nodeType, index);

    if (pNode == nullptr)
    {
        throw TraceException("Cannot find node.");
    }

    return pNode;
}

int64_t CycleViewImpl::GetNodeSize(NodeType nodeType, int index) const
{
    const auto pMeta = GetPointerToMeta(nodeType, index);

    if (pMeta == nullptr)
    {
        throw TraceException("Cannot find node.");
    }

    return pMeta->size;
}

int CycleViewImpl::GetNodeCount(NodeType nodeType) const
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

const BasicInfoNode* CycleViewImpl::GetBasicInfoNode() const
{
    CheckNodeSizeEqualTo(NodeType::BasicInfo, 0, sizeof(BasicInfoNode));
    return reinterpret_cast<const BasicInfoNode*>(GetNode(NodeType::BasicInfo, 0));
}

const FpRegNode* CycleViewImpl::GetFpRegNode() const
{
    CheckNodeSizeEqualTo(NodeType::FpReg, 0, sizeof(FpRegNode));
    return reinterpret_cast<const FpRegNode*>(GetNode(NodeType::FpReg, 0));
}

const IntReg32Node* CycleViewImpl::GetIntReg32Node() const
{
    CheckNodeSizeEqualTo(NodeType::IntReg32, 0, sizeof(IntReg32Node));
    return reinterpret_cast<const IntReg32Node*>(GetNode(NodeType::IntReg32, 0));
}

const IntReg64Node* CycleViewImpl::GetIntReg64Node() const
{
    CheckNodeSizeEqualTo(NodeType::IntReg64, 0, sizeof(IntReg64Node));
    return reinterpret_cast<const IntReg64Node*>(GetNode(NodeType::IntReg64, 0));
}

const Pc32Node* CycleViewImpl::GetPc32Node() const
{
    CheckNodeSizeEqualTo(NodeType::Pc32, 0, sizeof(Pc32Node));
    return reinterpret_cast<const Pc32Node*>(GetNode(NodeType::Pc32, 0));
}

const Pc64Node* CycleViewImpl::GetPc64Node() const
{
    CheckNodeSizeEqualTo(NodeType::Pc64, 0, sizeof(Pc64Node));
    return reinterpret_cast<const Pc64Node*>(GetNode(NodeType::Pc64, 0));
}

const Trap32Node* CycleViewImpl::GetTrap32Node() const
{
    CheckNodeSizeEqualTo(NodeType::Trap32, 0, sizeof(Trap32Node));
    return reinterpret_cast<const Trap32Node*>(GetNode(NodeType::Trap32, 0));
}

const Trap64Node* CycleViewImpl::GetTrap64Node() const
{
    CheckNodeSizeEqualTo(NodeType::Trap64, 0, sizeof(Trap64Node));
    return reinterpret_cast<const Trap64Node*>(GetNode(NodeType::Trap64, 0));
}

const MemoryAccessNode* CycleViewImpl::GetMemoryAccessNode(int index) const
{
    CheckNodeSizeEqualTo(NodeType::MemoryAccess, index, sizeof(MemoryAccessNode));
    return reinterpret_cast<const MemoryAccessNode*>(GetNode(NodeType::MemoryAccess, index));
}

const IoNode* CycleViewImpl::GetIoNode() const
{
    CheckNodeSizeEqualTo(NodeType::Io, 0, sizeof(IoNode));
    return reinterpret_cast<const IoNode*>(GetNode(NodeType::Io, 0));
}

const void* CycleViewImpl::GetMemoryNode() const
{
    CheckNodeSizeGreaterThan(NodeType::Memory, 0, 0);
    return GetNode(NodeType::Memory, 0);
}

void CycleViewImpl::CheckNodeSizeEqualTo(NodeType nodeType, int index, size_t size) const
{
    const auto nodeSize = GetNodeSize(nodeType, index);

    if (nodeSize < 0 || static_cast<size_t>(nodeSize) != size)
    {
        throw TraceException("node size is incorrect.");
    }
}

void CycleViewImpl::CheckNodeSizeGreaterThan(NodeType nodeType, int index, size_t size) const
{
    const auto nodeSize = GetNodeSize(nodeType, index);

    if (nodeSize < 0 || static_cast<size_t>(nodeSize) <= size)
    {
        throw TraceException("node size is incorrect.");
    }
}

const CycleHeader* CycleViewImpl::GetPointerToHeader() const
{
    return reinterpret_cast<const CycleHeader*>(m_pData);
}

const CycleMetaNode* CycleViewImpl::GetPointerToMeta(uint32_t index) const
{
    assert(0 <= index);
    assert(index < GetPointerToHeader()->metaCount);

    const auto metaNodes = reinterpret_cast<const CycleMetaNode*>(GetPointerToHeader() + 1);

    return &metaNodes[index];
}

const CycleMetaNode* CycleViewImpl::GetPointerToMeta(NodeType nodeType, int index) const
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

const void* CycleViewImpl::GetPointerToNode(NodeType nodeType, int index) const
{
    const auto pMeta = GetPointerToMeta(nodeType, index);

    if (pMeta == nullptr)
    {
        return nullptr;
    }

    return reinterpret_cast<const uint8_t*>(m_pData) + pMeta->offset;
}

}}
