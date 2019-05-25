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

        return isPhysical ? node->physicalPc : m_Impl.GetPc32Node()->virtualPc;
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

}}
