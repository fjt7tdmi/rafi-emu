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

#include <rvtrace/reader.h>

TraceCycleReader::TraceCycleReader(const void* buffer, int64_t bufferSize)
{
    m_pData = buffer;
    m_BufferSize = bufferSize;
}

int64_t TraceCycleReader::GetOffsetOfPreviousCycle() const
{
    return GetPointerToHeader()->prev;
}

int64_t TraceCycleReader::GetOffsetOfNextCycle() const
{
    return GetPointerToHeader()->next;
}

const void* TraceCycleReader::GetNode(NodeType nodeType) const
{
    auto pNode = GetPointerToNode(nodeType);

    if (pNode == nullptr)
    {
        throw TraceCycleException("Cannot find node.");
    }

    return pNode;
}

int64_t TraceCycleReader::GetNodeSize(NodeType nodeType) const
{
    auto pMeta = GetPointerToMeta(nodeType);

    if (pMeta == nullptr)
    {
        throw TraceCycleException("Cannot find node.");
    }

    return pMeta->size;
}

bool TraceCycleReader::IsNodeExist(NodeType nodeType) const
{
    return GetPointerToMeta(nodeType) != nullptr;
}

const BasicInfoNode* TraceCycleReader::GetBasicInfoNode() const
{
    CheckNodeSizeEqualTo(NodeType::BasicInfo, sizeof(BasicInfoNode));
    return reinterpret_cast<const BasicInfoNode*>(GetNode(NodeType::BasicInfo));
}

const Pc32Node* TraceCycleReader::GetPc32Node() const
{
    CheckNodeSizeEqualTo(NodeType::Pc32, sizeof(Pc32Node));
    return reinterpret_cast<const Pc32Node*>(GetNode(NodeType::Pc32));
}

const Pc64Node* TraceCycleReader::GetPc64Node() const
{
    CheckNodeSizeEqualTo(NodeType::Pc64, sizeof(Pc64Node));
    return reinterpret_cast<const Pc64Node*>(GetNode(NodeType::Pc64));
}

const IntReg32Node* TraceCycleReader::GetIntReg32Node() const
{
    CheckNodeSizeEqualTo(NodeType::IntReg32, sizeof(IntReg32Node));
    return reinterpret_cast<const IntReg32Node*>(GetNode(NodeType::IntReg32));
}

const IntReg64Node* TraceCycleReader::GetIntReg64Node() const
{
    CheckNodeSizeEqualTo(NodeType::IntReg64, sizeof(IntReg64Node));
    return reinterpret_cast<const IntReg64Node*>(GetNode(NodeType::IntReg64));
}

const Csr32NodeHeader* TraceCycleReader::GetCsr32Node() const
{
    CheckNodeSizeGreaterThan(NodeType::Csr32, sizeof(Csr32NodeHeader));
    return reinterpret_cast<const Csr32NodeHeader*>(GetNode(NodeType::Csr32));
}

const Csr64NodeHeader* TraceCycleReader::GetCsr64Node() const
{
    CheckNodeSizeGreaterThan(NodeType::Csr64, sizeof(Csr64NodeHeader));
    return reinterpret_cast<const Csr64NodeHeader*>(GetNode(NodeType::Csr64));
}

const Trap32Node* TraceCycleReader::GetTrap32Node() const
{
    CheckNodeSizeEqualTo(NodeType::Trap32, sizeof(Trap32Node));
    return reinterpret_cast<const Trap32Node*>(GetNode(NodeType::Trap32));
}

const Trap64Node* TraceCycleReader::GetTrap64Node() const
{
    CheckNodeSizeEqualTo(NodeType::Trap64, sizeof(Trap64Node));
    return reinterpret_cast<const Trap64Node*>(GetNode(NodeType::Trap64));
}

const MemoryAccess32Node* TraceCycleReader::GetMemoryAccess32Node() const
{
    CheckNodeSizeEqualTo(NodeType::MemoryAccess32, sizeof(MemoryAccess32Node));
    return reinterpret_cast<const MemoryAccess32Node*>(GetNode(NodeType::MemoryAccess32));
}

const MemoryAccess64Node* TraceCycleReader::GetMemoryAccess64Node() const
{
    CheckNodeSizeEqualTo(NodeType::MemoryAccess64, sizeof(MemoryAccess64Node));
    return reinterpret_cast<const MemoryAccess64Node*>(GetNode(NodeType::MemoryAccess64));
}

const IoNode* TraceCycleReader::GetIoNode() const
{
    CheckNodeSizeEqualTo(NodeType::Io, sizeof(IoNode));
    return reinterpret_cast<const IoNode*>(GetNode(NodeType::Io));
}

const MemoryNodeHeader* TraceCycleReader::GetMemoryNode() const
{
    CheckNodeSizeGreaterThan(NodeType::Memory, sizeof(MemoryNodeHeader));
    return reinterpret_cast<const MemoryNodeHeader*>(GetNode(NodeType::Memory));
}

void TraceCycleReader::CheckNodeSizeEqualTo(NodeType nodeType, size_t size) const
{
    if (!(GetNodeSize(nodeType) == size))
    {
        throw TraceCycleException("node size is incorrect.");
    }
}

void TraceCycleReader::CheckNodeSizeGreaterThan(NodeType nodeType, size_t size) const
{
    if (!(GetNodeSize(nodeType) > size))
    {
        throw TraceCycleException("node size is incorrect.");
    }
}

const TraceCycleHeader* TraceCycleReader::GetPointerToHeader() const
{
    return reinterpret_cast<const TraceCycleHeader*>(m_pData);
}

const TraceCycleMetaNode* TraceCycleReader::GetPointerToMeta(int32_t index) const
{
    assert(0 <= index);
    assert(index < GetPointerToHeader()->metaCount);

    auto metaNodes = reinterpret_cast<const TraceCycleMetaNode*>(GetPointerToHeader() + 1);

    return &metaNodes[index];
}

const TraceCycleMetaNode* TraceCycleReader::GetPointerToMeta(NodeType nodeType) const
{
    const auto metaCount = GetPointerToHeader()->metaCount;

    for (int i = 0; i < metaCount; i++)
    {
        auto pMeta = GetPointerToMeta(i);

        if (pMeta->nodeType == nodeType)
        {
            return pMeta;
        }
    }

    return nullptr;
}

const void* TraceCycleReader::GetPointerToNode(NodeType nodeType) const
{
    auto pMeta = GetPointerToMeta(nodeType);

    if (pMeta == nullptr)
    {
        return nullptr;
    }

    return reinterpret_cast<const uint8_t*>(m_pData) + pMeta->offset;
}
