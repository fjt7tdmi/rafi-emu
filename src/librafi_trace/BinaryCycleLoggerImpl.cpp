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

#include "BinaryCycleLoggerImpl.h"

namespace rafi { namespace trace {

namespace {
    size_t DefaultBufferSize = 4096;
}

BinaryCycleLoggerImpl::BinaryCycleLoggerImpl(uint32_t cycle, const XLEN& xlen, uint64_t pc)
{
    m_pBuffer = malloc(DefaultBufferSize);
    m_BufferSize = DefaultBufferSize;

    NodeBasic node{ cycle, xlen, pc };
    Add(node);
}

BinaryCycleLoggerImpl::~BinaryCycleLoggerImpl()
{
    free(m_pBuffer);
}

void BinaryCycleLoggerImpl::Add(const NodeIntReg32& node)
{
    AddData(NodeId_IN, &node, sizeof(node));
}

void BinaryCycleLoggerImpl::Add(const NodeIntReg64& node)
{
    AddData(NodeId_IN, &node, sizeof(node));
}

void BinaryCycleLoggerImpl::Add(const NodeFpReg& node)
{
    AddData(NodeId_FP, &node, sizeof(node));
}

void BinaryCycleLoggerImpl::Add(const NodeIo& node)
{
    AddData(NodeId_IO, &node, sizeof(node));
}

void BinaryCycleLoggerImpl::Add(const NodeOpEvent& node)
{
    AddData(NodeId_OP, &node, sizeof(node));
}

void BinaryCycleLoggerImpl::Add(const NodeTrapEvent& node)
{
    AddData(NodeId_TR, &node, sizeof(node));
}

void BinaryCycleLoggerImpl::Add(const NodeMemoryEvent& node)
{
    AddData(NodeId_MA, &node, sizeof(node));
}

void BinaryCycleLoggerImpl::Break()
{
    AddData(NodeId_BR, nullptr, 0);
}

void* BinaryCycleLoggerImpl::GetData()
{
    return m_pBuffer;
}

size_t BinaryCycleLoggerImpl::GetDataSize() const
{
    return m_DataSize;
}

void BinaryCycleLoggerImpl::Add(const NodeBasic& node)
{
    AddData(NodeId_BA, &node, sizeof(node));
}

void BinaryCycleLoggerImpl::AddData(uint16_t nodeId, const void* pNode, size_t nodeSize)
{
    if (m_DataSize + sizeof(NodeHeader) + nodeSize > m_BufferSize)
    {
        throw TraceException("Buffer overflow @ BinaryCycleLoggerImpl.\n");
    }

    if (nodeSize > UINT32_MAX)
    {
        throw TraceException("Node size is too large @ BinaryCycleLoggerImpl.\n");
    }

    NodeHeader header{ nodeId, 0, static_cast<uint32_t>(nodeSize) };

    std::memcpy(reinterpret_cast<uint8_t*>(m_pBuffer) + m_DataSize, &header, sizeof(header));
    m_DataSize += sizeof(header);

    if (pNode)
    {
        std::memcpy(reinterpret_cast<uint8_t*>(m_pBuffer) + m_DataSize, pNode, nodeSize);
        m_DataSize += nodeSize;
    }
}

}}
