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

#include "Bus.h"
#include "MemoryMap.h"

namespace rafi { namespace bus {

int8_t Bus::GetInt8(PhysicalAddress address)
{
    if (IsMemoryAddress(address, sizeof(int8_t)))
    {
        auto offset = ConvertToMemoryOffset(address);
        return m_pMemory->GetInt8(offset);
    }
    else if (IsIoAddress(address, sizeof(int8_t)))
    {
        const auto location = ConvertToIoOffset(address);
        return location.first->GetInt8(location.second);
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
        auto offset = ConvertToMemoryOffset(address);
        m_pMemory->SetInt8(offset, value);
    }
    else if (IsIoAddress(address, sizeof(int8_t)))
    {
        const auto location = ConvertToIoOffset(address);
        location.first->SetInt8(location.second, value);
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
        auto offset = ConvertToMemoryOffset(address);
        return m_pMemory->GetInt16(offset);
    }
    else if (IsIoAddress(address, sizeof(int16_t)))
    {
        const auto location = ConvertToIoOffset(address);
        return location.first->GetInt16(location.second);
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
        auto offset = ConvertToMemoryOffset(address);
        m_pMemory->SetInt16(offset, value);
    }
    else if (IsIoAddress(address, sizeof(int16_t)))
    {
        const auto location = ConvertToIoOffset(address);
        location.first->SetInt16(location.second, value);
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
        auto offset = ConvertToMemoryOffset(address);
        return m_pMemory->GetInt8(offset);
    }
    else if (IsIoAddress(address, sizeof(int32_t)))
    {
        const auto location = ConvertToIoOffset(address);
        return location.first->GetInt8(location.second);
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
        auto offset = ConvertToMemoryOffset(address);
        m_pMemory->SetInt32(offset, value);
    }
    else if (IsIoAddress(address, sizeof(int32_t)))
    {
        const auto location = ConvertToIoOffset(address);
        location.first->SetInt32(location.second, value);
    }
    else
    {
        throw InvalidAccessException(address);
    }
}

int Bus::ConvertToMemoryOffset(PhysicalAddress address) const
{
    if (MemoryAddr <= address && address < MemoryAddr + m_pMemory->GetSize())
    {
        return static_cast<int>(address - MemoryAddr);
    }
    else if (MemoryMirrorAddr <= address && address < MemoryMirrorAddr + m_pMemory->GetSize())
    {
        return static_cast<int>(address - MemoryMirrorAddr);
    }
    else
    {
        throw InvalidAccessException(address);
    }
}

bool Bus::IsMemoryAddress(PhysicalAddress address, int accessSize) const
{
    const auto low = address;
    const auto high = address + accessSize - 1;

    if (MemoryAddr <= low && high < MemoryAddr + m_pUart->GetSize())
    {
        return true;
    }
    else if (MemoryMirrorAddr <= low && high < MemoryMirrorAddr + m_pTimer->GetSize())
    {
        return true;
    }
    else
    {
        return false;
    }
}

Bus::Location Bus::ConvertToIoOffset(PhysicalAddress address) const
{
    if (UartAddr <= address && address < UartAddr + m_pUart->GetSize())
    {
        return std::make_pair(m_pUart, static_cast<int>(address - UartAddr));
    }
    else if (TimerAddr <= address && address < TimerAddr + m_pTimer->GetSize())
    {
        return std::make_pair(m_pTimer, static_cast<int>(address - TimerAddr));
    }
    else
    {
        throw InvalidAccessException(address);
    }
}

bool Bus::IsIoAddress(PhysicalAddress address, int accessSize) const
{
    const auto low = address;
    const auto high = address + accessSize - 1;

    if (UartAddr <= low && high < UartAddr + m_pUart->GetSize())
    {
        return true;
    }
    else if (TimerAddr <= low && high < TimerAddr + m_pTimer->GetSize())
    {
        return true;
    }
    else
    {
        return false;
    }
}

}}
