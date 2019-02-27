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

#include <rafi/trace.h>

#include "CycleBuilderImpl.h"

namespace rafi { namespace trace {

CycleBuilderImpl::CycleBuilderImpl(const CycleConfig& config)
    : m_Config(config)
{
    const auto size = CalculateDataSize();
    if (size > SIZE_MAX)
    {
        throw TraceException("Overflow.");
    }

    m_pData = malloc(static_cast<size_t>(size));
    m_DataSize = size;

    GetPointerToHeader()->footerOffset = size - sizeof(CycleFooter);
    GetPointerToHeader()->metaCount = config.GetTotalNodeCount();
    GetPointerToHeader()->reserved = 0;

    GetPointerToFooter()->headerOffset = size - sizeof(CycleFooter);

    InitializeMetaNodes();
}

CycleBuilderImpl::~CycleBuilderImpl()
{
    free(m_pData);
}

void* CycleBuilderImpl::GetData()
{
    return m_pData;
}

int64_t CycleBuilderImpl::GetDataSize()
{
    return m_DataSize;
}

int64_t CycleBuilderImpl::GetNodeSize(NodeType nodeType)
{
    assert(m_Config.GetNodeCount(nodeType) == 1);

    return GetNodeSize(nodeType, 0);
}

int64_t CycleBuilderImpl::GetNodeSize(NodeType nodeType, int index)
{
    const auto pMeta = GetPointerToMeta(nodeType, index);

    if (pMeta == nullptr)
    {
        throw TraceException("Cannot find node.");
    }

    return pMeta->size;
}

void* CycleBuilderImpl::GetPointerToNode(NodeType nodeType)
{
    assert(m_Config.GetNodeCount(nodeType) == 1);

    return GetPointerToNode(nodeType, 0);
}

void* CycleBuilderImpl::GetPointerToNode(NodeType nodeType, int index)
{
    const auto pMeta = GetPointerToMeta(nodeType, index);

    if (pMeta == nullptr)
    {
        return nullptr;
    }

    return reinterpret_cast<uint8_t*>(m_pData) + pMeta->offset;
}

void CycleBuilderImpl::SetNode(NodeType nodeType, const void* buffer, int64_t bufferSize)
{
    SetNode(nodeType, 0, buffer, bufferSize);
}

void CycleBuilderImpl::SetNode(NodeType nodeType, int index, const void* buffer, int64_t bufferSize)
{
    const auto pMeta = GetPointerToMeta(nodeType, index);

    if (pMeta == nullptr)
    {
        throw TraceException("Cannot find node.");
    }

    if (pMeta->size != bufferSize)
    {
        throw TraceException("Buffer size not matched.");
    }

    if (!(0 <= bufferSize && bufferSize < SIZE_MAX))
    {
        throw TraceException("Overflow.");
    }

    const auto size = static_cast<size_t>(bufferSize);

    std::memcpy(GetPointerToNode(nodeType, index), buffer, size);
}

void CycleBuilderImpl::SetNode(const BasicInfoNode& node)
{
    SetNode(NodeType::BasicInfo, &node, sizeof(node));
}

void CycleBuilderImpl::SetNode(const FpRegNode& node)
{
    SetNode(NodeType::FpReg, &node, sizeof(node));
}

void CycleBuilderImpl::SetNode(const IntReg32Node& node)
{
    SetNode(NodeType::IntReg32, &node, sizeof(node));
}

void CycleBuilderImpl::SetNode(const IntReg64Node& node)
{
    SetNode(NodeType::IntReg64, &node, sizeof(node));
}

void CycleBuilderImpl::SetNode(const Pc32Node& node)
{
    SetNode(NodeType::Pc32, &node, sizeof(node));
}

void CycleBuilderImpl::SetNode(const Pc64Node& node)
{
    SetNode(NodeType::Pc64, &node, sizeof(node));
}

void CycleBuilderImpl::SetNode(const Trap32Node& node)
{
    SetNode(NodeType::Trap32, &node, sizeof(node));
}

void CycleBuilderImpl::SetNode(const Trap64Node& node)
{
    SetNode(NodeType::Trap64, &node, sizeof(node));
}

void CycleBuilderImpl::SetNode(const MemoryAccessNode& node, int index)
{
    SetNode(NodeType::MemoryAccess, index, &node, sizeof(node));
}

void CycleBuilderImpl::SetNode(const IoNode& node)
{
    SetNode(NodeType::Io, &node, sizeof(node));
}

int64_t CycleBuilderImpl::CalculateDataSize()
{
    const int totalNodeCount = m_Config.GetTotalNodeCount();

    int64_t size = sizeof(CycleHeader) + totalNodeCount * sizeof(CycleMetaNode);

    for (int nodeType = 0; nodeType < NodeTypeMax; nodeType++)
    {
        size += GetProperNodeSize(static_cast<NodeType>(nodeType)) * m_Config.GetNodeCount(nodeType);
    }

    size += sizeof(CycleFooter);

    return size;
}

void CycleBuilderImpl::InitializeMetaNodes()
{
    const int totalNodeCount = m_Config.GetTotalNodeCount();

    int nodeIndex = 0;
    int64_t offset = sizeof(CycleHeader) + totalNodeCount * sizeof(CycleMetaNode);

    for (int nodeType = 0; nodeType < NodeTypeMax; nodeType++)
    {
        for (int i = 0; i < m_Config.GetNodeCount(nodeType); i++)
        {
            auto p = GetPointerToMeta(nodeIndex);

            p->nodeType = static_cast<NodeType>(nodeType);
            p->offset = offset;
            p->size = GetProperNodeSize(static_cast<NodeType>(nodeType));
            p->reserved = 0;

            offset += GetProperNodeSize(static_cast<NodeType>(nodeType));
            nodeIndex += 1;
        }
    }
}

int64_t CycleBuilderImpl::GetProperNodeSize(NodeType nodeType)
{
    switch (nodeType)
    {
    case NodeType::BasicInfo:
        return sizeof(BasicInfoNode);
    case NodeType::FpReg:
        return sizeof(FpRegNode);
    case NodeType::IntReg32:
        return sizeof(IntReg32Node);
    case NodeType::IntReg64:
        return sizeof(IntReg64Node);
    case NodeType::Pc32:
        return sizeof(Pc32Node);
    case NodeType::Pc64:
        return sizeof(Pc64Node);
    case NodeType::Csr32:
        return sizeof(Csr32Node) * m_Config.GetCsrCount();
    case NodeType::Csr64:
        return sizeof(Csr64Node) * m_Config.GetCsrCount();
    case NodeType::Trap32:
        return sizeof(Trap32Node);
    case NodeType::Trap64:
        return sizeof(Trap64Node);
    case NodeType::MemoryAccess:
        return sizeof(MemoryAccessNode);
    case NodeType::Io:
        return sizeof(IoNode);
    case NodeType::Memory:
        return m_Config.GetRamSize();
    default:
        return 0;
    }
}

CycleHeader* CycleBuilderImpl::GetPointerToHeader()
{
    return reinterpret_cast<CycleHeader*>(m_pData);
}

CycleFooter* CycleBuilderImpl::GetPointerToFooter()
{
    auto offset = GetPointerToHeader()->footerOffset;

    return reinterpret_cast<CycleFooter*>(reinterpret_cast<uint8_t*>(m_pData) + offset);
}

CycleMetaNode* CycleBuilderImpl::GetPointerToMeta(uint32_t index)
{
    assert(0 <= index);
    assert(index < GetPointerToHeader()->metaCount);

    auto metaNodes = reinterpret_cast<CycleMetaNode*>(GetPointerToHeader() + 1);

    return &metaNodes[index];
}

CycleMetaNode* CycleBuilderImpl::GetPointerToMeta(NodeType nodeType, int index)
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

}}