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

#include <rafi/Macro.h>

#include "Timer.h"

namespace rafi { namespace timer {

namespace {

int32_t GetHigh32(uint64_t value)
{
    return static_cast<int32_t>(value >> 32);
}

int32_t GetLow32(uint64_t value)
{
    return static_cast<int32_t>(value & 0xffffffff);
}

void SetHigh32(uint64_t* pOut, int32_t value)
{
    (*pOut) &= 0xffffffff00000000LL;
    (*pOut) |= value;
}

void SetLow32(uint64_t* pOut, int32_t value)
{
    (*pOut) &= 0x00000000ffffffffLL;
    (*pOut) |= (static_cast<int64_t>(value) << 32);
}

}

int8_t Timer::GetInt8(int)
{
    ABORT();
}

void Timer::SetInt8(int, int8_t)
{
    ABORT();
}

int16_t Timer::GetInt16(int)
{
    ABORT();
}

void Timer::SetInt16(int, int16_t)
{
    ABORT();
}

int32_t Timer::GetInt32(int address)
{
    switch (address)
    {
    case Address_TimeLow:
        return GetLow32(m_Time);
    case Address_TimeHigh:
        return GetHigh32(m_Time);
    case Address_TimeCmpLow:
        return GetLow32(m_TimeCmp);
    case Address_TimeCmpHigh:
        return GetHigh32(m_TimeCmp);
    default:
        ABORT();
    }
}

void Timer::SetInt32(int address, int32_t value)
{
    switch (address)
    {
    case Address_TimeLow:
        SetLow32(&m_Time, value);
    case Address_TimeHigh:
        SetHigh32(&m_Time, value);
    case Address_TimeCmpLow:
        SetLow32(&m_TimeCmp, value);
    case Address_TimeCmpHigh:
        SetHigh32(&m_TimeCmp, value);
    default:
        ABORT();
    }
}

void Timer::ProcessCycle()
{
    m_Time++;
}

}}
