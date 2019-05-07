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
#include <cstdio>
#include <cstring>

#include <rafi/emu.h>

#include "Clint.h"

namespace rafi { namespace emu { namespace io {

void Clint::Read(void* pOutBuffer, size_t size, uint64_t address)
{
    RAFI_EMU_CHECK_ACCESS(address, size, GetSize());

    switch (address)
    {
    case ADDR_MSIP:
        ReadMsip(pOutBuffer, size);
        break;
    case ADDR_MTIME:
        ReadTime(pOutBuffer, size);
        break;
    case ADDR_MTIMECMP:
        if (size > sizeof(m_TimeCmp))
        {
            RAFI_EMU_ERROR("[Clint] Read size (%zd byte) for mtimecmp is invalid.\n", size);
        }
        std::memcpy(pOutBuffer, &m_TimeCmp, size);
        break;
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }
}

void Clint::Write(const void* pBuffer, size_t size, uint64_t address)
{
    RAFI_EMU_CHECK_ACCESS(address, size, GetSize());

    switch (address)
    {
    case ADDR_MSIP:
        WriteMsip(pBuffer, size);
        break;
    case ADDR_MTIME:
        WriteTime(pBuffer, size);
        break;
    case ADDR_MTIMECMP:
        if (size > sizeof(m_TimeCmp))
        {
            RAFI_EMU_ERROR("[Clint] Write size (%zd byte) for mtimecmp is invalid.\n", size);
        }
        std::memcpy(&m_TimeCmp, pBuffer, size);
        break;
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }
}

int Clint::GetSize() const
{
    return RegisterSpaceSize;
}

// This method is for timer interrupt.
// For software interrupt, clint modifies csr directly.
bool Clint::IsInterruptRequested() const
{
    return m_pProcessor->ReadTime() >= m_TimeCmp;
}

void Clint::ProcessCycle()
{
}

void Clint::RegisterProcessor(cpu::Processor* pProcessor)
{
    m_pProcessor = pProcessor;
}

void Clint::ReadMsip(void* pOutBuffer, size_t size)
{
    const auto mip = m_pProcessor->ReadInterruptPending();
    const uint32_t value = mip.GetMember<xip_t::MSIP>();

    if (size != sizeof(value))
    {
        RAFI_EMU_ERROR("[Clint] Read size (%zd byte) for msip is invalid .\n", size);
    }

    std::memcpy(pOutBuffer, &value, size);
}

void Clint::ReadTime(void* pOutBuffer, size_t size)
{
    const auto value = m_pProcessor->ReadTime();

    if (size != sizeof(value))
    {
        RAFI_EMU_ERROR("[Clint] Read size (%zd byte) for mtime is invalid .\n", size);
    }

    std::memcpy(pOutBuffer, &value, size);
}

void Clint::WriteMsip(const void* pBuffer, size_t size)
{
    uint32_t value;

    if (size != sizeof(value))
    {
        RAFI_EMU_ERROR("[Clint] Write size (%zd byte) for msip is invalid .\n", size);
    }

    std::memcpy(&value, pBuffer, size);

    auto mip = m_pProcessor->ReadInterruptPending();
    mip.SetMember<xip_t::MSIP>(value & 0x1);
    m_pProcessor->WriteInterruptPending(mip);
}

void Clint::WriteTime(const void* pBuffer, size_t size)
{
    uint64_t value;

    if (size != sizeof(value))
    {
        RAFI_EMU_ERROR("[Clint] Write size (%zd byte) for mtime is invalid .\n", size);
    }

    std::memcpy(&value, pBuffer, size);

    m_pProcessor->WriteTime(value);
}

}}}
