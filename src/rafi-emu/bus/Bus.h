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

#include <vector>
#include <utility>

#include <rafi/emu.h>

#include "../io/IIo.h"
#include "../mem/IMemory.h"

namespace rafi { namespace emu { namespace bus {

struct MemoryInfo
{
    mem::IMemory* pMemory;
    PhysicalAddress address;
    int size;
};

struct IoInfo
{
    io::IIo* pIo;
    PhysicalAddress address;
    int size;
};

struct MemoryLocation
{
    mem::IMemory* pMemory;
    int offset;
};

struct IoLocation
{
    io::IIo* pIo;
    int offset;
};

class Bus
{
public:
    void Read(void* pOutBuffer, size_t size, PhysicalAddress address);
    void Write(const void* pBuffer, size_t size, PhysicalAddress address);

    int8_t ReadInt8(PhysicalAddress address);
    int16_t ReadInt16(PhysicalAddress address);
    int32_t ReadInt32(PhysicalAddress address);
    int64_t ReadInt64(PhysicalAddress address);

    void WriteInt8(PhysicalAddress address, int8_t value);
    void WriteInt16(PhysicalAddress address, int16_t value);
    void WriteInt32(PhysicalAddress address, int32_t value);
    void WriteInt64(PhysicalAddress address, int64_t value);

    void RegisterMemory(mem::IMemory* pMemory, PhysicalAddress address, int size);
    void RegisterIo(io::IIo* pIo, PhysicalAddress address, int size);

    MemoryLocation ConvertToMemoryLocation(PhysicalAddress address) const;
    bool IsMemoryAddress(PhysicalAddress address, int accessSize) const;

    IoLocation ConvertToIoLocation(PhysicalAddress address) const;
    bool IsIoAddress(PhysicalAddress address, int accessSize) const;

private:
    std::vector<MemoryInfo> m_MemoryList;
    std::vector<IoInfo> m_IoList;
};

}}}
