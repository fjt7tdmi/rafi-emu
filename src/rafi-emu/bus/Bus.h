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
    paddr_t address;
    size_t size;
};

struct IoInfo
{
    io::IIo* pIo;
    paddr_t address;
    size_t size;
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
    void Read(void* pOutBuffer, size_t size, paddr_t address);
    void Write(const void* pBuffer, size_t size, paddr_t address);

    uint8_t ReadUInt8(paddr_t address);
    uint16_t ReadUInt16(paddr_t address);
    uint32_t ReadUInt32(paddr_t address);
    uint64_t ReadUInt64(paddr_t address);

    void WriteUInt8(paddr_t address, uint8_t value);
    void WriteUInt16(paddr_t address, uint16_t value);
    void WriteUInt32(paddr_t address, uint32_t value);
    void WriteUInt64(paddr_t address, uint64_t value);

    void RegisterMemory(mem::IMemory* pMemory, paddr_t address, size_t size);
    void RegisterIo(io::IIo* pIo, paddr_t address, size_t size);

    MemoryLocation ConvertToMemoryLocation(paddr_t address) const;
    bool IsMemoryAddress(paddr_t address, size_t accessSize) const;

    IoLocation ConvertToIoLocation(paddr_t address) const;
    bool IsIoAddress(paddr_t address, size_t accessSize) const;

private:
    std::vector<MemoryInfo> m_MemoryList;
    std::vector<IoInfo> m_IoList;
};

}}}
