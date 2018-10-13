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

#pragma once

#include <utility>

#include <rafi/BasicTypes.h>

#include "IBusSlave.h"
#include "IIo.h"

#include "../mem/Memory.h"
#include "../uart/Uart.h"
#include "../timer/Timer.h"

namespace rafi { namespace bus {

class Bus
{
public:
    Bus(mem::Memory* pMemory, uart::Uart* pUart, timer::Timer* pTimer)
        : m_pMemory(pMemory)
        , m_pUart(pUart)
        , m_pTimer(pTimer)
    {
    }

    int8_t GetInt8(PhysicalAddress address);
    void SetInt8(PhysicalAddress address, int8_t value);

    int16_t GetInt16(PhysicalAddress address);
    void SetInt16(PhysicalAddress address, int16_t value);

    int32_t GetInt32(PhysicalAddress address);
    void SetInt32(PhysicalAddress address, int32_t value);

    int ConvertToMemoryOffset(PhysicalAddress address) const;
    bool IsMemoryAddress(PhysicalAddress address, int accessSize) const;

private:
    using Location = std::pair<IIo*, int>;

    Location ConvertToIoOffset(PhysicalAddress address) const;
    bool IsIoAddress(PhysicalAddress address, int accessSize) const;

    mem::Memory* m_pMemory;
    uart::Uart* m_pUart;
    timer::Timer* m_pTimer;
};

}}
