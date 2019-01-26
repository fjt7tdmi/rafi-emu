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
#include <cstdlib>
#include <cstring>

#include <rvtrace/writer.h>

#include "TraceCycleBuilderImpl.h"

using namespace std;

namespace rvtrace {

TraceCycleBuilderImpl::TraceCycleBuilderImpl(int32_t flags, int csrCount, int ramSize)
{
    const auto size = CalculateDataSize(flags, csrCount, ramSize);

    if (size > SIZE_MAX)
    {
        throw TraceCycleException("Overflow.");
    }

    m_pData = malloc(static_cast<size_t>(size));
    m_DataSize = size;

    GetPointerToHeader()->footerOffset = size - sizeof(TraceCycleFooter);
    GetPointerToHeader()->metaCount = CountValidFlags(flags);
    GetPointerToHeader()->reserved = 0;

    GetPointerToFooter()->headerOffset = size - sizeof(TraceCycleFooter);

    InitializeMetaNodes(flags, csrCount, ramSize);
}

TraceCycleBuilderImpl::~TraceCycleBuilderImpl()
{
    free(m_pData);
}

void* TraceCycleBuilderImpl::GetData()
{
    return m_pData;
}

int64_t TraceCycleBuilderImpl::GetDataSize()
{
    return m_DataSize;
}

int64_t TraceCycleBuilderImpl::GetNodeSize(NodeType nodeType)
{
    const auto pMeta = GetPointerToMeta(nodeType);

    if (pMeta == nullptr)
    {
        throw TraceCycleException("Cannot find node.");
    }

    return pMeta->size;
}

void* TraceCycleBuilderImpl::GetPointerToNode(NodeType nodeType)
{
    const auto pMeta = GetPointerToMeta(nodeType);

    if (pMeta == nullptr)
    {
        return nullptr;
    }

    return reinterpret_cast<uint8_t*>(m_pData) + pMeta->offset;
}

void TraceCycleBuilderImpl::SetNode(NodeType nodeType, const void* buffer, int64_t bufferSize)
{
    const auto pMeta = GetPointerToMeta(nodeType);

    if (pMeta == nullptr)
    {
        throw TraceCycleException("Cannot find node.");
    }

    if (pMeta->size != bufferSize)
    {
        throw TraceCycleException("Buffer size not matched.");
    }

    if (!(0 <= bufferSize && bufferSize < SIZE_MAX))
    {
        throw TraceCycleException("Overflow.");
    }

    const auto size = static_cast<size_t>(bufferSize);

    std::memcpy(GetPointerToNode(nodeType), buffer, size);
}

void TraceCycleBuilderImpl::SetNode(const BasicInfoNode& node)
{
    SetNode(NodeType::BasicInfo, &node, sizeof(node));
}

void TraceCycleBuilderImpl::SetNode(const Pc32Node& node)
{
    SetNode(NodeType::Pc32, &node, sizeof(node));
}

void TraceCycleBuilderImpl::SetNode(const Pc64Node& node)
{
    SetNode(NodeType::Pc64, &node, sizeof(node));
}

void TraceCycleBuilderImpl::SetNode(const IntReg32Node& node)
{
    SetNode(NodeType::IntReg32, &node, sizeof(node));
}

void TraceCycleBuilderImpl::SetNode(const IntReg64Node& node)
{
    SetNode(NodeType::IntReg64, &node, sizeof(node));
}

void TraceCycleBuilderImpl::SetNode(const FpRegNode& node)
{
    SetNode(NodeType::FpReg, &node, sizeof(node));
}

void TraceCycleBuilderImpl::SetNode(const Trap32Node& node)
{
    SetNode(NodeType::Trap32, &node, sizeof(node));
}

void TraceCycleBuilderImpl::SetNode(const Trap64Node& node)
{
    SetNode(NodeType::Trap64, &node, sizeof(node));
}

void TraceCycleBuilderImpl::SetNode(const MemoryAccess32Node& node)
{
    SetNode(NodeType::MemoryAccess32, &node, sizeof(node));
}

void TraceCycleBuilderImpl::SetNode(const MemoryAccess64Node& node)
{
    SetNode(NodeType::MemoryAccess64, &node, sizeof(node));
}

void TraceCycleBuilderImpl::SetNode(const IoNode& node)
{
    SetNode(NodeType::Io, &node, sizeof(node));
}

int64_t TraceCycleBuilderImpl::CalculateDataSize(int32_t flags, int csrCount, int ramSize)
{
    int64_t size = sizeof(TraceCycleHeader) + sizeof(TraceCycleFooter);

    size += sizeof(TraceCycleMetaNode) * CountValidFlags(flags);

#define GET_SIZE_FOR_FLAG(flag_) (((flags & NodeFlag_##flag_) != 0) ? GetProperNodeSize(NodeType:: flag_ , csrCount, ramSize) : 0)
    size += GET_SIZE_FOR_FLAG(BasicInfo);
    size += GET_SIZE_FOR_FLAG(Pc32);
    size += GET_SIZE_FOR_FLAG(Pc64);
    size += GET_SIZE_FOR_FLAG(IntReg32);
    size += GET_SIZE_FOR_FLAG(IntReg64);
    size += GET_SIZE_FOR_FLAG(Csr32);
    size += GET_SIZE_FOR_FLAG(Csr64);
    size += GET_SIZE_FOR_FLAG(Trap32);
    size += GET_SIZE_FOR_FLAG(Trap64);
    size += GET_SIZE_FOR_FLAG(MemoryAccess32);
    size += GET_SIZE_FOR_FLAG(MemoryAccess64);
    size += GET_SIZE_FOR_FLAG(Io);
    size += GET_SIZE_FOR_FLAG(Memory);
    size += GET_SIZE_FOR_FLAG(FpReg);
#undef GET_SIZE_FOR_FLAG

    return size;
}

int32_t TraceCycleBuilderImpl::CountValidFlags(int32_t flags)
{
    int32_t count = 0;

#define ONE_IF_VALID(flag_) (((flags & NodeFlag_##flag_) != 0) ? 1 : 0)
    count += ONE_IF_VALID(BasicInfo);
    count += ONE_IF_VALID(Pc32);
    count += ONE_IF_VALID(Pc64);
    count += ONE_IF_VALID(IntReg32);
    count += ONE_IF_VALID(IntReg64);
    count += ONE_IF_VALID(Csr32);
    count += ONE_IF_VALID(Csr64);
    count += ONE_IF_VALID(Trap32);
    count += ONE_IF_VALID(Trap64);
    count += ONE_IF_VALID(MemoryAccess32);
    count += ONE_IF_VALID(MemoryAccess64);
    count += ONE_IF_VALID(Io);
    count += ONE_IF_VALID(Memory);
    count += ONE_IF_VALID(FpReg);
#undef ONE_IF_VALID

    return count;
}

void TraceCycleBuilderImpl::InitializeMetaNodes(int32_t flags, int csrCount, int ramSize)
{
    int32_t index = 0;
    int64_t offset = sizeof(TraceCycleHeader) + CountValidFlags(flags) * sizeof(TraceCycleMetaNode);

    if (flags & NodeFlag_BasicInfo)
    {
        InitializeMetaNode(index, NodeType::BasicInfo, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::BasicInfo, csrCount, ramSize);
        index++;
    }
    if (flags & NodeFlag_Pc32)
    {
        InitializeMetaNode(index, NodeType::Pc32, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::Pc32, csrCount, ramSize);
        index++;
    }
    if (flags & NodeFlag_Pc64)
    {
        InitializeMetaNode(index, NodeType::Pc64, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::Pc64, csrCount, ramSize);
        index++;
    }
    if (flags & NodeFlag_IntReg32)
    {
        InitializeMetaNode(index, NodeType::IntReg32, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::IntReg32, csrCount, ramSize);
        index++;
    }
    if (flags & NodeFlag_IntReg64)
    {
        InitializeMetaNode(index, NodeType::IntReg64, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::IntReg64, csrCount, ramSize);
        index++;
    }
    if (flags & NodeFlag_Csr32)
    {
        InitializeMetaNode(index, NodeType::Csr32, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::Csr32, csrCount, ramSize);
        index++;
    }
    if (flags & NodeFlag_Csr64)
    {
        InitializeMetaNode(index, NodeType::Csr64, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::Csr64, csrCount, ramSize);
        index++;
    }
    if (flags & NodeFlag_Trap32)
    {
        InitializeMetaNode(index, NodeType::Trap32, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::Trap32, csrCount, ramSize);
        index++;
    }
    if (flags & NodeFlag_Trap64)
    {
        InitializeMetaNode(index, NodeType::Trap64, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::Trap64, csrCount, ramSize);
        index++;
    }
    if (flags & NodeFlag_MemoryAccess32)
    {
        InitializeMetaNode(index, NodeType::MemoryAccess32, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::MemoryAccess32, csrCount, ramSize);
        index++;
    }
    if (flags & NodeFlag_MemoryAccess64)
    {
        InitializeMetaNode(index, NodeType::MemoryAccess64, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::MemoryAccess64, csrCount, ramSize);
        index++;
    }
    if (flags & NodeFlag_Io)
    {
        InitializeMetaNode(index, NodeType::Io, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::Io, csrCount, ramSize);
        index++;
    }
    if (flags & NodeFlag_Memory)
    {
        InitializeMetaNode(index, NodeType::Memory, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::Memory, csrCount, ramSize);
        index++;
    }
    if (flags & NodeFlag_FpReg)
    {
        InitializeMetaNode(index, NodeType::FpReg, offset, csrCount, ramSize);

        offset += GetProperNodeSize(NodeType::FpReg, csrCount, ramSize);
        index++;
    }

    assert(index == CountValidFlags(flags));
}

void TraceCycleBuilderImpl::InitializeMetaNode(int32_t index, NodeType nodeType, int64_t offset, int csrCount, int ramSize)
{
    GetPointerToMeta(index)->nodeType = nodeType;
    GetPointerToMeta(index)->offset = offset;
    GetPointerToMeta(index)->size = GetProperNodeSize(nodeType, csrCount, ramSize);
    GetPointerToMeta(index)->reserved = 0;
}

int64_t TraceCycleBuilderImpl::GetProperNodeSize(NodeType nodeType, int csrCount, int ramSize)
{
    switch (nodeType)
    {
    case NodeType::BasicInfo:
        return sizeof(BasicInfoNode);
    case NodeType::Pc32:
        return sizeof(Pc32Node);
    case NodeType::Pc64:
        return sizeof(Pc64Node);
    case NodeType::IntReg32:
        return sizeof(IntReg32Node);
    case NodeType::IntReg64:
        return sizeof(IntReg64Node);
    case NodeType::Csr32:
        return sizeof(Csr32Node) * csrCount;
    case NodeType::Csr64:
        return sizeof(Csr64Node) * csrCount;
    case NodeType::Trap32:
        return sizeof(Trap32Node);
    case NodeType::Trap64:
        return sizeof(Trap64Node);
    case NodeType::MemoryAccess32:
        return sizeof(MemoryAccess32Node);
    case NodeType::MemoryAccess64:
        return sizeof(MemoryAccess64Node);
    case NodeType::Io:
        return sizeof(IoNode);
    case NodeType::Memory:
        return ramSize;
    case NodeType::FpReg:
        return sizeof(FpRegNode);
    default:
        throw TraceCycleException("Unknown NodeType.");
    }
}

TraceCycleHeader* TraceCycleBuilderImpl::GetPointerToHeader()
{
    return reinterpret_cast<TraceCycleHeader*>(m_pData);
}

TraceCycleFooter* TraceCycleBuilderImpl::GetPointerToFooter()
{
    auto offset = GetPointerToHeader()->footerOffset;

    return reinterpret_cast<TraceCycleFooter*>(reinterpret_cast<uint8_t*>(m_pData) + offset);
}

TraceCycleMetaNode* TraceCycleBuilderImpl::GetPointerToMeta(int32_t index)
{
    assert(0 <= index);
    assert(index < GetPointerToHeader()->metaCount);

    auto metaNodes = reinterpret_cast<TraceCycleMetaNode*>(GetPointerToHeader() + 1);

    return &metaNodes[index];
}

TraceCycleMetaNode* TraceCycleBuilderImpl::GetPointerToMeta(NodeType nodeType)
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

}
