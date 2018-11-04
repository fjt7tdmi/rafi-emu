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

#include <rafi/Exception.h>
#include <rafi/MemoryMap.h>

#include "Bus.h"

namespace rafi { namespace emu { namespace bus {

int8_t Bus::GetInt8(PhysicalAddress address)
{
    if (IsMemoryAddress(address, sizeof(int8_t)))
    {
        const auto location = ConvertToMemoryLocation(address);
        return location.pMemory->GetInt8(location.offset);
    }
    else if (IsIoAddress(address, sizeof(int8_t)))
    {
        const auto location = ConvertToIoLocation(address);
        return location.pIo->GetInt8(location.offset);
    }
    else
    {
        throw InvalidAccessException(address);
    }
}

void Bus::SetInt8(PhysicalAddress address, int8_t value)
{
    if (IsMemoryAddress(address, sizeof(int8_t)))
    {
        const auto location = ConvertToMemoryLocation(address);
        location.pMemory->SetInt8(location.offset, value);
    }
    else if (IsIoAddress(address, sizeof(int8_t)))
    {
        const auto location = ConvertToIoLocation(address);
        location.pIo->SetInt8(location.offset, value);
    }
    else
    {
        throw InvalidAccessException(address);
    }
}

int16_t Bus::GetInt16(PhysicalAddress address)
{
    if (IsMemoryAddress(address, sizeof(int16_t)))
    {
        const auto location = ConvertToMemoryLocation(address);
        return location.pMemory->GetInt16(location.offset);
    }
    else if (IsIoAddress(address, sizeof(int16_t)))
    {
        const auto location = ConvertToIoLocation(address);
        return location.pIo->GetInt16(location.offset);
    }
    else
    {
        throw InvalidAccessException(address);
    }
}

void Bus::SetInt16(PhysicalAddress address, int16_t value)
{
    if (IsMemoryAddress(address, sizeof(int16_t)))
    {
        const auto location = ConvertToMemoryLocation(address);
        location.pMemory->SetInt16(location.offset, value);
    }
    else if (IsIoAddress(address, sizeof(int16_t)))
    {
        const auto location = ConvertToIoLocation(address);
        location.pIo->SetInt16(location.offset, value);
    }
    else
    {
        throw InvalidAccessException(address);
    }
}

int32_t Bus::GetInt32(PhysicalAddress address)
{
    if (IsMemoryAddress(address, sizeof(int32_t)))
    {
        const auto location = ConvertToMemoryLocation(address);
        return location.pMemory->GetInt32(location.offset);
    }
    else if (IsIoAddress(address, sizeof(int32_t)))
    {
        const auto location = ConvertToIoLocation(address);
        return location.pIo->GetInt32(location.offset);
    }
    else
    {
        throw InvalidAccessException(address);
    }
}

void Bus::SetInt32(PhysicalAddress address, int32_t value)
{
    if (IsMemoryAddress(address, sizeof(int32_t)))
    {
        const auto location = ConvertToMemoryLocation(address);
        location.pMemory->SetInt32(location.offset, value);
    }
    else if (IsIoAddress(address, sizeof(int32_t)))
    {
        const auto location = ConvertToIoLocation(address);
        location.pIo->SetInt32(location.offset, value);
    }
    else
    {
        throw InvalidAccessException(address);
    }
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

    throw InvalidAccessException(address);
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

    throw InvalidAccessException(address);
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
