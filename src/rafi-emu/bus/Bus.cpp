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

#include <rafi/emu.h>

#include "Bus.h"

// Suppress VC warning for printf "%16lx"
#pragma warning(disable:4477)

namespace rafi { namespace emu { namespace bus {

void Bus::Read(void* pOutBuffer, size_t size, paddr_t address)
{
    if (IsMemoryAddress(address, size))
    {
        const auto location = ConvertToMemoryLocation(address);
        return location.pMemory->Read(pOutBuffer, size, location.offset);
    }
    else if (IsIoAddress(address, sizeof(uint8_t)))
    {
        const auto location = ConvertToIoLocation(address);
        return location.pIo->Read(pOutBuffer, size, location.offset);
    }
    else
    {
        RAFI_EMU_ERROR("Invalid addresss: 0x%016llx\n", static_cast<unsigned long long>(address));
    }
}

void Bus::Write(const void* pBuffer, size_t size, paddr_t address)
{
    if (IsMemoryAddress(address, sizeof(int8_t)))
    {
        const auto location = ConvertToMemoryLocation(address);
        location.pMemory->Write(pBuffer, size, location.offset);
    }
    else if (IsIoAddress(address, sizeof(int8_t)))
    {
        const auto location = ConvertToIoLocation(address);
        location.pIo->Write(pBuffer, size, location.offset);
    }
    else
    {
        RAFI_EMU_ERROR("Invalid addresss: 0x%016llx\n", static_cast<unsigned long long>(address));
    }
}

uint8_t Bus::ReadUInt8(paddr_t address)
{
    int8_t value;
    Read(&value, sizeof(value), address);

    return value;
}

uint16_t Bus::ReadUInt16(paddr_t address)
{
    int16_t value;
    Read(&value, sizeof(value), address);

    return value;
}

uint32_t Bus::ReadUInt32(paddr_t address)
{
    uint32_t value;
    Read(&value, sizeof(value), address);

    return value;
}

uint64_t Bus::ReadUInt64(paddr_t address)
{
    int64_t value;
    Read(&value, sizeof(value), address);

    return value;
}

void Bus::WriteUInt8(paddr_t address, uint8_t value)
{
    Write(&value, sizeof(value), address);
}

void Bus::WriteUInt16(paddr_t address, uint16_t value)
{
    Write(&value, sizeof(value), address);
}

void Bus::WriteUInt32(paddr_t address, uint32_t value)
{
    Write(&value, sizeof(value), address);
}

void Bus::WriteUInt64(paddr_t address, uint64_t value)
{
    Write(&value, sizeof(value), address);
}

void Bus::RegisterMemory(mem::IMemory* pMemory, paddr_t address, size_t size)
{
    MemoryInfo info { pMemory, address, size };
    m_MemoryList.push_back(info);
}

void Bus::RegisterIo(io::IIo* pIo, paddr_t address, size_t size)
{
    IoInfo info { pIo, address, size };
    m_IoList.push_back(info);
}

bool Bus::IsValidAddress(paddr_t address, size_t accessSize) const
{
    return IsMemoryAddress(address, accessSize) && IsIoAddress(address, accessSize);
}

bool Bus::IsMemoryAddress(paddr_t address, size_t accessSize) const
{
    const auto low = address;
    const auto high = address + accessSize - 1;

    for (const auto& location: m_MemoryList)
    {
        if (location.address <= low && high < location.address + location.size)
        {
            return true;
        }
    }

    return false;
}

bool Bus::IsIoAddress(paddr_t address, size_t accessSize) const
{
    const auto low = address;
    const auto high = address + accessSize - 1;

    for (const auto& location: m_IoList)
    {
        if (location.address <= low && high < location.address + location.size)
        {
            return true;
        }
    }

    return false;
}

MemoryLocation Bus::ConvertToMemoryLocation(paddr_t address) const
{
    for (const auto& location: m_MemoryList)
    {
        if (location.address <= address && address < location.address + location.size)
        {
            MemoryLocation ret;

            ret.pMemory = location.pMemory;
            ret.offset = static_cast<int>(address - location.address);

            return ret;
        }
    }

    RAFI_EMU_ERROR("Invalid addresss: 0x%016llx\n", static_cast<unsigned long long>(address));
}

IoLocation Bus::ConvertToIoLocation(paddr_t address) const
{
    for (const auto& location: m_IoList)
    {
        if (location.address <= address && address < location.address + location.size)
        {
            IoLocation ret;

            ret.pIo = location.pIo;
            ret.offset = static_cast<int>(address - location.address);

            return ret;
        }
    }

    RAFI_EMU_ERROR("Invalid addresss: 0x%016llx\n", static_cast<unsigned long long>(address));
}

}}}
