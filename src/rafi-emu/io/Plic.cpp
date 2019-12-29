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
#include <cinttypes>
#include <cstdio>
#include <cstring>

#include <rafi/emu.h>

#include "Plic.h"

namespace rafi { namespace emu { namespace io {

void Plic::Read(void* pOutBuffer, size_t size, uint64_t address)
{
    RAFI_EMU_CHECK_ACCESS(address, size, GetSize());

    if (size == sizeof(uint32_t))
    {
        const uint32_t value = ReadUInt32(address);

        std::memcpy(pOutBuffer, &value, size);
    }
    else if (size == sizeof(uint64_t))
    {
        const uint32_t low = ReadUInt32(address);
        const uint32_t high = ReadUInt32(address + sizeof(uint32_t));

        const uint64_t value = (static_cast<uint64_t>(high) << 32) | static_cast<uint64_t>(low);

        std::memcpy(pOutBuffer, &value, size);
    }
    else
    {
        RAFI_EMU_ERROR("[Plic] Read size (%zd byte) is invalid. (address: 0x%" PRIx64 ")\n", size, address);
    }
}

void Plic::Write(const void* pBuffer, size_t size, uint64_t address)
{
    RAFI_EMU_CHECK_ACCESS(address, size, GetSize());

    if (size == sizeof(uint32_t))
    {
        uint32_t value;
        std::memcpy(&value, pBuffer, size);

        WriteUInt32(address, value);
    }
    else if (size == sizeof(uint64_t))
    {
        uint64_t value;
        std::memcpy(&value, pBuffer, size);

        WriteUInt32(address, static_cast<uint32_t>(value));
        WriteUInt32(address + sizeof(uint32_t), static_cast<uint32_t>(value >> 32));
    }
    else
    {
        RAFI_EMU_ERROR("[Plic] Write size (%zd byte) is invalid. (address: 0x%" PRIx64 ")\n", size, address);
    }
}

int Plic::GetSize() const
{
    return RegisterSpaceSize;
}

bool Plic::IsInterruptRequested() const
{
    // PLIC main logic is not implemented.
    return false;
}

uint32_t Plic::ReadUInt32(uint64_t address)
{
    if (ADDR_PRIORITY_BEGIN <= address && address < ADDR_PRIORITY_END)
    {
        return m_Priorities[(address - ADDR_PRIORITY_BEGIN) / sizeof(uint32_t)];
    }
    else if (ADDR_PENDING_BEGIN <= address && address < ADDR_PENDING_END)
    {
        return m_Pendings[(address - ADDR_PENDING_BEGIN) / sizeof(uint32_t)];
    }
    else if (ADDR_ENABLE_M_BEGIN <= address && address < ADDR_ENABLE_M_END)
    {
        return m_MachineInterruptEnables[(address - ADDR_ENABLE_M_BEGIN) / sizeof(uint32_t)];
    }
    else if (ADDR_ENABLE_S_BEGIN <= address && address < ADDR_ENABLE_S_END)
    {
        return m_SupervisorInterruptEnables[(address - ADDR_ENABLE_S_BEGIN) / sizeof(uint32_t)];
    }
    else if (address == ADDR_THRESHOLD_M)
    {
        return m_MachineThreshold;
    }
    else if (address == ADDR_THRESHOLD_S)
    {
        return m_SupervisorThreshold;
    }
    else
    {
        RAFI_EMU_ERROR("[Plic] Read address (0x%" PRIx64 ") is invalid .\n", address);
    }
}

void Plic::WriteUInt32(uint64_t address, uint32_t value)
{
    if (ADDR_PRIORITY_BEGIN <= address && address < ADDR_PRIORITY_END)
    {
        m_Priorities[(address - ADDR_PRIORITY_BEGIN) / sizeof(uint32_t)] = value & PriorityMask;
    }
    else if (ADDR_ENABLE_M_BEGIN <= address && address < ADDR_ENABLE_M_END)
    {
        m_MachineInterruptEnables[(address - ADDR_ENABLE_M_BEGIN) / sizeof(uint32_t)] = value;
    }
    else if (ADDR_ENABLE_S_BEGIN <= address && address < ADDR_ENABLE_S_END)
    {
        m_SupervisorInterruptEnables[(address - ADDR_ENABLE_S_BEGIN) / sizeof(uint32_t)] = value;
    }
    else if (address == ADDR_THRESHOLD_M)
    {
        m_MachineThreshold = value & PriorityMask;
    }
    else if (address == ADDR_THRESHOLD_S)
    {
        m_SupervisorThreshold = value & PriorityMask;
    }
    else
    {
        RAFI_EMU_ERROR("[Plic] Write address (0x%" PRIx64 ") is invalid.\n", address);
    }
}

}}}
