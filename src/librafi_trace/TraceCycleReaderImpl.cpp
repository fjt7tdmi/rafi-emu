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

#include "TraceCycleReaderImpl.h"

namespace rafi { namespace trace {

TraceCycleReaderImpl::TraceCycleReaderImpl(const void* buffer, int64_t bufferSize)
{
    m_pData = buffer;
    m_BufferSize = bufferSize;
}

const void* TraceCycleReaderImpl::GetNode(NodeType nodeType, int index) const
{
    const auto pNode = GetPointerToNode(nodeType, index);

    if (pNode == nullptr)
    {
        throw TraceCycleException("Cannot find node.");
    }

    return pNode;
}

int64_t TraceCycleReaderImpl::GetNodeSize(NodeType nodeType, int index) const
{
    const auto pMeta = GetPointerToMeta(nodeType, index);

    if (pMeta == nullptr)
    {
        throw TraceCycleException("Cannot find node.");
    }

    return pMeta->size;
}

int TraceCycleReaderImpl::GetNodeCount(NodeType nodeType) const
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

const BasicInfoNode* TraceCycleReaderImpl::GetBasicInfoNode() const
{
    CheckNodeSizeEqualTo(NodeType::BasicInfo, 0, sizeof(BasicInfoNode));
    return reinterpret_cast<const BasicInfoNode*>(GetNode(NodeType::BasicInfo, 0));
}

const FpRegNode* TraceCycleReaderImpl::GetFpRegNode() const
{
    CheckNodeSizeEqualTo(NodeType::FpReg, 0, sizeof(FpRegNode));
    return reinterpret_cast<const FpRegNode*>(GetNode(NodeType::FpReg, 0));
}

const IntReg32Node* TraceCycleReaderImpl::GetIntReg32Node() const
{
    CheckNodeSizeEqualTo(NodeType::IntReg32, 0, sizeof(IntReg32Node));
    return reinterpret_cast<const IntReg32Node*>(GetNode(NodeType::IntReg32, 0));
}

const IntReg64Node* TraceCycleReaderImpl::GetIntReg64Node() const
{
    CheckNodeSizeEqualTo(NodeType::IntReg64, 0, sizeof(IntReg64Node));
    return reinterpret_cast<const IntReg64Node*>(GetNode(NodeType::IntReg64, 0));
}

const Pc32Node* TraceCycleReaderImpl::GetPc32Node() const
{
    CheckNodeSizeEqualTo(NodeType::Pc32, 0, sizeof(Pc32Node));
    return reinterpret_cast<const Pc32Node*>(GetNode(NodeType::Pc32, 0));
}

const Pc64Node* TraceCycleReaderImpl::GetPc64Node() const
{
    CheckNodeSizeEqualTo(NodeType::Pc64, 0, sizeof(Pc64Node));
    return reinterpret_cast<const Pc64Node*>(GetNode(NodeType::Pc64, 0));
}

const Csr32Node* TraceCycleReaderImpl::GetCsr32Node() const
{
    CheckNodeSizeGreaterThan(NodeType::Csr32, 0, sizeof(Csr32Node));
    return reinterpret_cast<const Csr32Node*>(GetNode(NodeType::Csr32, 0));
}

const Csr64Node* TraceCycleReaderImpl::GetCsr64Node() const
{
    CheckNodeSizeGreaterThan(NodeType::Csr64, 0, sizeof(Csr64Node));
    return reinterpret_cast<const Csr64Node*>(GetNode(NodeType::Csr64, 0));
}

const Trap32Node* TraceCycleReaderImpl::GetTrap32Node() const
{
    CheckNodeSizeEqualTo(NodeType::Trap32, 0, sizeof(Trap32Node));
    return reinterpret_cast<const Trap32Node*>(GetNode(NodeType::Trap32, 0));
}

const Trap64Node* TraceCycleReaderImpl::GetTrap64Node() const
{
    CheckNodeSizeEqualTo(NodeType::Trap64, 0, sizeof(Trap64Node));
    return reinterpret_cast<const Trap64Node*>(GetNode(NodeType::Trap64, 0));
}

const MemoryAccessNode* TraceCycleReaderImpl::GetMemoryAccessNode(int index) const
{
    CheckNodeSizeEqualTo(NodeType::MemoryAccess, index, sizeof(MemoryAccessNode));
    return reinterpret_cast<const MemoryAccessNode*>(GetNode(NodeType::MemoryAccess, index));
}

const IoNode* TraceCycleReaderImpl::GetIoNode() const
{
    CheckNodeSizeEqualTo(NodeType::Io, 0, sizeof(IoNode));
    return reinterpret_cast<const IoNode*>(GetNode(NodeType::Io, 0));
}

const void* TraceCycleReaderImpl::GetMemoryNode() const
{
    CheckNodeSizeGreaterThan(NodeType::Memory, 0, 0);
    return GetNode(NodeType::Memory, 0);
}

void TraceCycleReaderImpl::CheckNodeSizeEqualTo(NodeType nodeType, int index, size_t size) const
{
    if (!(GetNodeSize(nodeType, index) == size))
    {
        throw TraceCycleException("node size is incorrect.");
    }
}

void TraceCycleReaderImpl::CheckNodeSizeGreaterThan(NodeType nodeType, int index, size_t size) const
{
    if (!(GetNodeSize(nodeType, index) > size))
    {
        throw TraceCycleException("node size is incorrect.");
    }
}

const TraceCycleHeader* TraceCycleReaderImpl::GetPointerToHeader() const
{
    return reinterpret_cast<const TraceCycleHeader*>(m_pData);
}

const TraceCycleMetaNode* TraceCycleReaderImpl::GetPointerToMeta(uint32_t index) const
{
    assert(0 <= index);
    assert(index < GetPointerToHeader()->metaCount);

    const auto metaNodes = reinterpret_cast<const TraceCycleMetaNode*>(GetPointerToHeader() + 1);

    return &metaNodes[index];
}

const TraceCycleMetaNode* TraceCycleReaderImpl::GetPointerToMeta(NodeType nodeType, int index) const
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

const void* TraceCycleReaderImpl::GetPointerToNode(NodeType nodeType, int index) const
{
    const auto pMeta = GetPointerToMeta(nodeType, index);

    if (pMeta == nullptr)
    {
        return nullptr;
    }

    return reinterpret_cast<const uint8_t*>(m_pData) + pMeta->offset;
}

}}
