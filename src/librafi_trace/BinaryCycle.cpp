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
    : m_Impl(buffer, bufferSize)
{
}

BinaryCycle::~BinaryCycle()
{
}

XLEN BinaryCycle::GetXLEN() const
{
    if (m_Impl.GetNodeCount(NodeType::Pc32) > 0)
    {
        return XLEN::XLEN32;
    }
    else if (m_Impl.GetNodeCount(NodeType::Pc64) > 0)
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

    return m_Impl.GetNodeCount(nodeType) > 0;
}

bool BinaryCycle::IsIntRegExist() const
{
    const auto nodeType = (GetXLEN() == XLEN::XLEN32) ? NodeType::IntReg32 : NodeType::IntReg64;

    return m_Impl.GetNodeCount(nodeType) > 0;
}

bool BinaryCycle::IsFpRegExist() const
{
    return m_Impl.GetNodeCount(NodeType::FpReg) > 0;
}

bool BinaryCycle::IsIoStateExist() const
{
    return m_Impl.GetNodeCount(NodeType::Io) > 0;
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
    return m_Impl.GetNodeCount(trace::NodeType::MemoryAccess);
}

int BinaryCycle::GetTrapEventCount() const
{
    return m_Impl.GetNodeCount(trace::NodeType::Trap32) + m_Impl.GetNodeCount(trace::NodeType::Trap64);
}

uint64_t BinaryCycle::GetPc(bool isPhysical) const
{
    if (GetXLEN() == XLEN::XLEN32)
    {
        auto node = m_Impl.GetPc32Node();

        return isPhysical ? node->physicalPc : m_Impl.GetPc32Node()->virtualPc;
    }
    else
    {
        auto node = m_Impl.GetPc64Node();

        return isPhysical ? node->physicalPc : m_Impl.GetPc64Node()->virtualPc;
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
        return m_Impl.GetIntReg32Node()->regs[index];
    }
    else
    {
        return m_Impl.GetIntReg64Node()->regs[index];
    }
}

uint64_t BinaryCycle::GetFpReg(int index) const
{
    if (!(0 <= index && index < FpRegCount))
    {
        throw TraceException("Specified index is out-of-range.");
    }

    return m_Impl.GetFpRegNode()->regs[index].u64.value;
}

void BinaryCycle::CopyIoState(IoState* pOutState) const
{
    pOutState->hostIo = m_Impl.GetIoNode()->hostIoValue;
    pOutState->reserved = 0;
}

void BinaryCycle::CopyNote(std::string* pOutNote) const
{
    *pOutNote = "(null)";
}

void BinaryCycle::CopyOpEvent(OpEvent* pOutEvent, int index) const
{
    (void)index;
    pOutEvent->insn = m_Impl.GetBasicInfoNode()->insn;
    pOutEvent->priv = m_Impl.GetBasicInfoNode()->privilegeLevel;
}

void BinaryCycle::CopyMemoryEvent(MemoryEvent* pOutEvent, int index) const
{
    std::memcpy(pOutEvent, m_Impl.GetMemoryAccessNode(index), sizeof(MemoryAccessNode));
}

void BinaryCycle::CopyTrapEvent(TrapEvent* pOutEvent, int index) const
{
    (void)index;

    if (GetXLEN() == XLEN::XLEN32)
    {
        auto pNode = m_Impl.GetTrap32Node();

        pOutEvent->cause = pNode->cause;
        pOutEvent->from = pNode->from;
        pOutEvent->to = pNode->to;
        pOutEvent->trapType = pNode->trapType;
        pOutEvent->trapValue = pNode->trapValue;
    }
    else
    {
        auto pNode = m_Impl.GetTrap64Node();

        pOutEvent->cause = pNode->cause;
        pOutEvent->from = pNode->from;
        pOutEvent->to = pNode->to;
        pOutEvent->trapType = pNode->trapType;
        pOutEvent->trapValue = pNode->trapValue;
    }
}

}}
