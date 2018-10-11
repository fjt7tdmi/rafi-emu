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

#include "../../Common/Exception.h"
#include "Bus.h"

//#define IGNORE_CONVERSION_ERROR

int8_t Bus::GetInt8(PhysicalAddress address)
{
    try
    {
        const auto location = Convert(address, sizeof(int32_t));
        return location.first->GetInt8(location.second);
    }
    catch (const InvalidAccessException& e)
    {
#ifdef IGNORE_CONVERSION_ERROR
        e.PrintMessage();
        return 0;
#else
        throw e;
#endif
    }
}

void Bus::SetInt8(PhysicalAddress address, int8_t value)
{
    try
    {
        const auto location = Convert(address, sizeof(int8_t));
        location.first->SetInt8(location.second, value);
    }
    catch (const InvalidAccessException& e)
    {
#ifdef IGNORE_CONVERSION_ERROR
        e.PrintMessage();
        return;
#else
        throw e;
#endif
    }
}

int16_t Bus::GetInt16(PhysicalAddress address)
{
    try
    {
        const auto location = Convert(address, sizeof(int32_t));
        return location.first->GetInt16(location.second);
    }
    catch (const InvalidAccessException& e)
    {
#ifdef IGNORE_CONVERSION_ERROR
        e.PrintMessage();
        return 0;
#else
        throw e;
#endif
    }
}

void Bus::SetInt16(PhysicalAddress address, int16_t value)
{
    try
    {
        const auto location = Convert(address, sizeof(int32_t));
        location.first->SetInt16(location.second, value);
    }
    catch (const InvalidAccessException& e)
    {
#ifdef IGNORE_CONVERSION_ERROR
        e.PrintMessage();
        return;
#else
        throw e;
#endif
    }
}

int32_t Bus::GetInt32(PhysicalAddress address)
{
    try
    {
        const auto location = Convert(address, sizeof(int32_t));
        return location.first->GetInt32(location.second);
    }
    catch (const InvalidAccessException& e)
    {
#ifdef IGNORE_CONVERSION_ERROR
        e.PrintMessage();
        return 0;
#else
        throw e;
#endif
    }
}

void Bus::SetInt32(PhysicalAddress address, int32_t value)
{
    try
    {
        const auto location = Convert(address, sizeof(int32_t));
        location.first->SetInt32(location.second, value);
    }
    catch (const InvalidAccessException& e)
    {
#ifdef IGNORE_CONVERSION_ERROR
        e.PrintMessage();
        return;
#else
        throw e;
#endif
    }
}

Bus::Location Bus::Convert(PhysicalAddress address, int accessSize) const
{
    const auto low = address;
    const auto high = address + accessSize - 1;

    if (UartAddr <= low && high < UartAddr + m_pUart->GetSize())
    {
        return std::make_pair(m_pUart, static_cast<int>(address - UartAddr));
    }
    else if (MemoryAddr <= low && high < MemoryAddr + m_pMemory->GetSize())
    {
        return std::make_pair(m_pMemory, static_cast<int>(address - MemoryAddr));
    }
    else if (MemoryMirrorAddr <= low && high < MemoryMirrorAddr + m_pMemory->GetSize())
    {
        return std::make_pair(m_pMemory, static_cast<int>(address - MemoryMirrorAddr));
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
