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

#include <rafi/Common.h>

#include "Bus.h"

// Suppress VC warning for printf "%16lx"
#pragma warning(disable:4477)

namespace rafi { namespace emu { namespace bus {

void Bus::Read(void* pOutBuffer, size_t size, PhysicalAddress address)
{
    if (IsMemoryAddress(address, size))
    {
        const auto location = ConvertToMemoryLocation(address);
        return location.pMemory->Read(pOutBuffer, size, location.offset);
    }
    else if (IsIoAddress(address, sizeof(int8_t)))
    {
        const auto location = ConvertToIoLocation(address);
        return location.pIo->Read(pOutBuffer, size, location.offset);
    }
    else
    {
        RAFI_EMU_ERROR("Invalid addresss: 0x%016lx\n", static_cast<uint64_t>(address));
    }
}

void Bus::Write(const void* pBuffer, size_t size, PhysicalAddress address)
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
        RAFI_EMU_ERROR("Invalid addresss: 0x%016lx\n", static_cast<uint64_t>(address));
    }
}

int8_t Bus::ReadInt8(PhysicalAddress address)
{
    int8_t value;
    Read(&value, sizeof(value), address);

    return value;
}

int16_t Bus::ReadInt16(PhysicalAddress address)
{
    int16_t value;
    Read(&value, sizeof(value), address);

    return value;
}

int32_t Bus::ReadInt32(PhysicalAddress address)
{
    int32_t value;
    Read(&value, sizeof(value), address);

    return value;
}

int64_t Bus::ReadInt64(PhysicalAddress address)
{
    int64_t value;
    Read(&value, sizeof(value), address);

    return value;
}

void Bus::WriteInt8(PhysicalAddress address, int8_t value)
{
    Write(&value, sizeof(value), address);
}

void Bus::WriteInt16(PhysicalAddress address, int16_t value)
{
    Write(&value, sizeof(value), address);
}

void Bus::WriteInt32(PhysicalAddress address, int32_t value)
{
    Write(&value, sizeof(value), address);
}

void Bus::WriteInt64(PhysicalAddress address, int64_t value)
{
    Write(&value, sizeof(value), address);
}

void Bus::RegisterMemory(mem::IMemory* pMemory, PhysicalAddress address, int size)
{
    MemoryInfo info { pMemory, address, size };
    m_MemoryList.push_back(info);
}

void Bus::RegisterIo(io::IIo* pIo, PhysicalAddress address, int size)
{
    IoInfo info { pIo, address, size };
    m_IoList.push_back(info);
}

MemoryLocation Bus::ConvertToMemoryLocation(PhysicalAddress address) const
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

    RAFI_EMU_ERROR("Invalid addresss: 0x%016lx\n", static_cast<uint64_t>(address));
}

bool Bus::IsMemoryAddress(PhysicalAddress address, int accessSize) const
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

IoLocation Bus::ConvertToIoLocation(PhysicalAddress address) const
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

    RAFI_EMU_ERROR("Invalid addresss: 0x%016lx\n", static_cast<uint64_t>(address));
}

bool Bus::IsIoAddress(PhysicalAddress address, int accessSize) const
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

}}}
