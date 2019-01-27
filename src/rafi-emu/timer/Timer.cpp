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

#include <rafi/Common.h>

#include "Timer.h"

namespace rafi { namespace emu { namespace timer {

void Timer::Read(void* pOutBuffer, size_t size, uint64_t address)
{
    if (!(0 <= address && address + size - 1 < RegSize))
    {
        throw FatalException(__FILE__, __LINE__);
    }

    int32_t value;

    if (size != sizeof(value))
    {
        throw FatalException(__FILE__, __LINE__);
    }

    switch (address)
    {
    case Address_TimeLow:
        value = GetLow32(m_Time);
        break;
    case Address_TimeHigh:
        value = GetHigh32(m_Time);
        break;
    case Address_TimeCmpLow:
        value = GetLow32(m_TimeCmp);
        break;
    case Address_TimeCmpHigh:
        value = GetHigh32(m_TimeCmp);
        break;
    default:
        throw FatalException(__FILE__, __LINE__);
    }

    std::memcpy(pOutBuffer, &value, sizeof(value));
}

void Timer::Write(const void* pBuffer, size_t size, uint64_t address)
{
    if (!(0 <= address && address + size - 1 < RegSize))
    {
        throw FatalException(__FILE__, __LINE__);
    }

    if (size != sizeof(int32_t))
    {
        throw FatalException(__FILE__, __LINE__);
    }

    int32_t value;
    std::memcpy(&value, pBuffer, sizeof(int32_t));

    switch (address)
    {
    case Address_TimeLow:
        SetLow32(&m_Time, value);
        break;
    case Address_TimeHigh:
        SetHigh32(&m_Time, value);
        break;
    case Address_TimeCmpLow:
        SetLow32(&m_TimeCmp, value);
        break;
    case Address_TimeCmpHigh:
        SetHigh32(&m_TimeCmp, value);
        break;
    default:
        throw FatalException(__FILE__, __LINE__);
    }
}

bool Timer::IsInterruptRequested() const
{
    return m_Time >= m_TimeCmp;
}

void Timer::ProcessCycle()
{
    m_Time++;
}

}}}
