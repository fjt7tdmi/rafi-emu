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

using namespace std;

TraceCycleBuilder::TraceCycleBuilder(int32_t flags)
{
    auto size = CalculateDataSize(flags);

    if (size > SIZE_MAX)
    {
        throw TraceCycleException("Overflow.");
    }

    m_pData = malloc(static_cast<size_t>(size));
    m_DataSize = size;

    GetPointerToHeader()->next = size;
    GetPointerToHeader()->prev = 0;
    GetPointerToHeader()->size = size;
    GetPointerToHeader()->metaCount = CountValidFlags(flags);
    GetPointerToHeader()->reserved = 0;

    InitializeMetaNodes(flags);
}

TraceCycleBuilder::~TraceCycleBuilder()
{
    free(m_pData);
}

void* TraceCycleBuilder::GetData()
{
    return m_pData;
}

int64_t TraceCycleBuilder::GetDataSize()
{
    return m_DataSize;
}

void TraceCycleBuilder::SetOffsetOfPreviousCycle(int64_t offset)
{
    GetPointerToHeader()->prev = offset;
}

void TraceCycleBuilder::SetOffsetOfNextCycle(int64_t offset)
{
    GetPointerToHeader()->next = offset;
}

void TraceCycleBuilder::SetNode(NodeType nodeType, const void* buffer, int64_t bufferSize)
{
    auto pMeta = GetPointerToMeta(nodeType);

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

    auto size = static_cast<size_t>(bufferSize);

    std::memcpy(GetPointerToNode(nodeType), buffer, size);
}

void TraceCycleBuilder::SetNode(const BasicInfoNode& node)
{
    SetNode(NodeType::BasicInfo, &node, sizeof(node));
}

void TraceCycleBuilder::SetNode(const Pc32Node& node)
{
    SetNode(NodeType::Pc32, &node, sizeof(node));
}

void TraceCycleBuilder::SetNode(const Pc64Node& node)
{
    SetNode(NodeType::Pc64, &node, sizeof(node));
}

void TraceCycleBuilder::SetNode(const IntReg32Node& node)
{
    SetNode(NodeType::IntReg32, &node, sizeof(node));
}

void TraceCycleBuilder::SetNode(const IntReg64Node& node)
{
    SetNode(NodeType::IntReg64, &node, sizeof(node));
}

void TraceCycleBuilder::SetNode(const Trap32Node& node)
{
    SetNode(NodeType::Trap32, &node, sizeof(node));
}

void TraceCycleBuilder::SetNode(const Trap64Node& node)
{
    SetNode(NodeType::Trap64, &node, sizeof(node));
}

void TraceCycleBuilder::SetNode(const MemoryAccess32Node& node)
{
    SetNode(NodeType::MemoryAccess32, &node, sizeof(node));
}

void TraceCycleBuilder::SetNode(const MemoryAccess64Node& node)
{
    SetNode(NodeType::MemoryAccess64, &node, sizeof(node));
}

void TraceCycleBuilder::SetNode(const IoNode& node)
{
    SetNode(NodeType::Io, &node, sizeof(node));
}

int64_t TraceCycleBuilder::CalculateDataSize(int32_t flags)
{
    int64_t size = sizeof(TraceCycleHeader);

    size += sizeof(TraceCycleMetaNode) * CountValidFlags(flags);

#define GET_SIZE_FOR_FLAG(flag_) (((flags & NodeFlag_##flag_) != 0) ? GetProperNodeSize(NodeType:: flag_) : 0)
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
#undef GET_SIZE_FOR_FLAG

    return size;
}

int32_t TraceCycleBuilder::CountValidFlags(int32_t flags)
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
#undef ONE_IF_VALID

    return count;
}

void TraceCycleBuilder::InitializeMetaNodes(int32_t flags)
{
    int32_t index = 0;
    int64_t offset = sizeof(TraceCycleHeader) + CountValidFlags(flags) * sizeof(TraceCycleMetaNode);

    if (flags & NodeFlag_BasicInfo)
    {
        InitializeMetaNode(index, NodeType::BasicInfo, offset);

        offset += GetProperNodeSize(NodeType::BasicInfo);
        index++;
    }
    if (flags & NodeFlag_Pc32)
    {
        InitializeMetaNode(index, NodeType::Pc32, offset);

        offset += GetProperNodeSize(NodeType::Pc32);
        index++;
    }
    if (flags & NodeFlag_Pc64)
    {
        InitializeMetaNode(index, NodeType::Pc64, offset);

        offset += GetProperNodeSize(NodeType::Pc64);
        index++;
    }
    if (flags & NodeFlag_IntReg32)
    {
        InitializeMetaNode(index, NodeType::IntReg32, offset);

        offset += GetProperNodeSize(NodeType::IntReg32);
        index++;
    }
    if (flags & NodeFlag_IntReg64)
    {
        InitializeMetaNode(index, NodeType::IntReg64, offset);

        offset += GetProperNodeSize(NodeType::IntReg64);
        index++;
    }
    if (flags & NodeFlag_Csr32)
    {
        InitializeMetaNode(index, NodeType::Csr32, offset);

        offset += GetProperNodeSize(NodeType::Csr32);
        index++;
    }
    if (flags & NodeFlag_Csr64)
    {
        InitializeMetaNode(index, NodeType::Csr64, offset);

        offset += GetProperNodeSize(NodeType::Csr64);
        index++;
    }
    if (flags & NodeFlag_Trap32)
    {
        InitializeMetaNode(index, NodeType::Trap32, offset);

        offset += GetProperNodeSize(NodeType::Trap32);
        index++;
    }
    if (flags & NodeFlag_Trap64)
    {
        InitializeMetaNode(index, NodeType::Trap64, offset);

        offset += GetProperNodeSize(NodeType::Trap64);
        index++;
    }
    if (flags & NodeFlag_MemoryAccess32)
    {
        InitializeMetaNode(index, NodeType::MemoryAccess32, offset);

        offset += GetProperNodeSize(NodeType::MemoryAccess32);
        index++;
    }
    if (flags & NodeFlag_MemoryAccess64)
    {
        InitializeMetaNode(index, NodeType::MemoryAccess64, offset);

        offset += GetProperNodeSize(NodeType::MemoryAccess64);
        index++;
    }
    if (flags & NodeFlag_Io)
    {
        InitializeMetaNode(index, NodeType::Io, offset);

        offset += GetProperNodeSize(NodeType::Io);
        index++;
    }
    if (flags & NodeFlag_Memory)
    {
        InitializeMetaNode(index, NodeType::Memory, offset);

        offset += GetProperNodeSize(NodeType::Memory);
        index++;
    }

    assert(index == CountValidFlags(flags));
}

void TraceCycleBuilder::InitializeMetaNode(int32_t index, NodeType nodeType, int64_t offset)
{
    GetPointerToMeta(index)->nodeType = nodeType;
    GetPointerToMeta(index)->offset = offset;
    GetPointerToMeta(index)->size = GetProperNodeSize(nodeType);
    GetPointerToMeta(index)->reserved = 0;
}

int64_t TraceCycleBuilder::GetProperNodeSize(NodeType nodeType)
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
        throw TraceCycleException("Not implemented (node size is variable).");
    case NodeType::Csr64:
        throw TraceCycleException("Not implemented (node size is variable).");
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
        throw TraceCycleException("Not implemented (node size is variable).");
    default:
        throw TraceCycleException("Unknown NodeType.");
    }
}

TraceCycleHeader* TraceCycleBuilder::GetPointerToHeader()
{
    return reinterpret_cast<TraceCycleHeader*>(m_pData);
}

TraceCycleMetaNode* TraceCycleBuilder::GetPointerToMeta(int32_t index)
{
    assert(0 <= index);
    assert(index < GetPointerToHeader()->metaCount);

    auto metaNodes = reinterpret_cast<TraceCycleMetaNode*>(GetPointerToHeader() + 1);

    return &metaNodes[index];
}

TraceCycleMetaNode* TraceCycleBuilder::GetPointerToMeta(NodeType nodeType)
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

void* TraceCycleBuilder::GetPointerToNode(NodeType nodeType)
{
    auto pMeta = GetPointerToMeta(nodeType);

    if (pMeta == nullptr)
    {
        return nullptr;
    }

    return reinterpret_cast<uint8_t*>(m_pData) + pMeta->offset;
}
