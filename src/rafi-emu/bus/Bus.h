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

#include <emu/BasicTypes.h>

#include "IBusSlave.h"

#include "../mem/Memory.h"
#include "../uart/Uart.h"

class Bus
{
public:
    Bus(Memory* pMemory, Uart* pUart)
        : m_pMemory(pMemory)
        , m_pUart(pUart)
    {
    }

    int8_t GetInt8(PhysicalAddress address);
    void SetInt8(PhysicalAddress address, int8_t value);

    int16_t GetInt16(PhysicalAddress address);
    void SetInt16(PhysicalAddress address, int16_t value);

    int32_t GetInt32(PhysicalAddress address);
    void SetInt32(PhysicalAddress address, int32_t value);

    // Memory Map
    static const PhysicalAddress UartAddr = 0x40000000;
    static const PhysicalAddress MemoryAddr = 0x80000000;
    static const PhysicalAddress MemoryMirrorAddr = 0xc0000000;

    static const PhysicalAddress HostIoAddr = 0x80001000;

    int ConvertToMemoryOffset(PhysicalAddress address) const;

private:
    using Location = std::pair<IBusSlave*, int>;

    Location Convert(PhysicalAddress address, int accessSize) const;

    Memory* m_pMemory;
    Uart* m_pUart;
};
