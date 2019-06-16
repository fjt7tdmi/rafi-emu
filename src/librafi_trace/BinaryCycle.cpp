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

std::unique_ptr<BinaryCycle> BinaryCycle::Parse(const void* buffer, size_t bufferSize)
{
    auto p = std::make_unique<BinaryCycle>();

    p->m_pBuffer = buffer;
    p->m_BufferSize = bufferSize;

    while (!p->m_Break)
    {
        p->m_Size += p->ParseNode(buffer, bufferSize);
    }

    return p;
}

BinaryCycle::BinaryCycle()
{
}

BinaryCycle::~BinaryCycle()
{
}

XLEN BinaryCycle::GetXLEN() const
{
    return m_pNodeBasic->xlen;
}

bool BinaryCycle::IsPcExist() const
{
    return m_pNodeBasic;
}

bool BinaryCycle::IsIntRegExist() const
{
    return (m_pNodeIntReg32 != nullptr) || (m_pNodeIntReg64 != nullptr);
}

bool BinaryCycle::IsFpRegExist() const
{
    return m_pNodeFpReg;
}

bool BinaryCycle::IsIoStateExist() const
{
    return m_pNodeIo;
}

bool BinaryCycle::IsNoteExist() const
{
    return nullptr;
}

size_t BinaryCycle::GetOpEventCount() const
{
    return m_OpEvents.size();
}

size_t BinaryCycle::GetMemoryEventCount() const
{
    return m_MemoryEvents.size();
}

size_t BinaryCycle::GetTrapEventCount() const
{
    return m_TrapEvents.size();
}

uint64_t BinaryCycle::GetPc(bool isPhysical) const
{
    if (isPhysical)
    {
        RAFI_NOT_IMPLEMENTED();
    }

    return m_pNodeBasic->pc;
}

uint64_t BinaryCycle::GetIntReg(size_t index) const
{
    if (m_pNodeIntReg32 != nullptr)
    {
        return m_pNodeIntReg32->regs[index];
    }
    else if (m_pNodeIntReg64 != nullptr)
    {
        return m_pNodeIntReg64->regs[index];
    }
    else
    {
        RAFI_NOT_IMPLEMENTED();
    }
}

uint64_t BinaryCycle::GetFpReg(size_t index) const
{
    return m_pNodeIntReg32->regs[index];
}

void BinaryCycle::CopyIo(NodeIo* pOutState) const
{
    std::memcpy(pOutState, m_pNodeIo, sizeof(NodeIo));
}

void BinaryCycle::CopyNote(std::string* pOutNote) const
{
    RAFI_NOT_IMPLEMENTED();
}

void BinaryCycle::CopyOpEvent(NodeOpEvent* pOutEvent, size_t index) const
{
    std::memcpy(pOutEvent, m_OpEvents[index], sizeof(NodeOpEvent));
}

void BinaryCycle::CopyMemoryEvent(NodeMemoryEvent* pOutEvent, size_t index) const
{
    std::memcpy(pOutEvent, m_MemoryEvents[index], sizeof(NodeMemoryEvent));
}

void BinaryCycle::CopyTrapEvent(NodeTrapEvent* pOutEvent, size_t index) const
{
    std::memcpy(pOutEvent, m_TrapEvents[index], sizeof(NodeTrapEvent));
}

size_t BinaryCycle::GetSize() const
{
    return m_Size;
}

size_t BinaryCycle::ParseNode(const void* buffer, size_t bufferSize)
{
    if (bufferSize < sizeof(NodeHeader))
    {
        throw TraceException("Broken data @ BinaryCycle\n");
    }

    const auto pHeader = reinterpret_cast<const NodeHeader*>(buffer);
    const auto size = sizeof(NodeHeader) + pHeader->nodeSize;

    if (bufferSize < size)
    {
        throw TraceException("Broken data @ BinaryCycle\n");
    }

    switch (pHeader->nodeId)
    {
    case NodeId_BA:
        m_pNodeBasic = reinterpret_cast<const NodeBasic*>(&pHeader[1]);
        break;
    case NodeId_BR:
        m_Break = true;
        break;
    case NodeId_FP:
        m_pNodeFpReg = reinterpret_cast<const NodeFpReg*>(&pHeader[1]);
        break;
    case NodeId_IN:
        if (!m_pNodeBasic)
        {
            throw TraceException("Detect IntReg node before Basic node\n");
        }
        else if (m_pNodeBasic->xlen == XLEN::XLEN32)
        {
            m_pNodeIntReg32 = reinterpret_cast<const NodeIntReg32*>(&pHeader[1]);
        }
        else if (m_pNodeBasic->xlen == XLEN::XLEN64)
        {
            m_pNodeIntReg64 = reinterpret_cast<const NodeIntReg64*>(&pHeader[1]);
        }
        else
        {
            RAFI_NOT_IMPLEMENTED();
        }
        break;
    case NodeId_IO:
        m_pNodeIo = reinterpret_cast<const NodeIo*>(&pHeader[1]);
        break;
    case NodeId_MA:
        m_MemoryEvents.push_back(reinterpret_cast<const NodeMemoryEvent*>(&pHeader[1]));
        break;
    case NodeId_OP:
        m_OpEvents.push_back(reinterpret_cast<const NodeOpEvent*>(&pHeader[1]));
        break;
    case NodeId_TR:
        m_TrapEvents.push_back(reinterpret_cast<const NodeTrapEvent*>(&pHeader[1]));
        break;
    default:
        throw TraceException("Unknown node id\n");
    }

    return size;
}

}}
