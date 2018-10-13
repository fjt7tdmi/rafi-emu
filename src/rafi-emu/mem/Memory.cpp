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
#include <cstdint>
#include <fstream>

#include <rvtrace/common.h>

#include <rafi/Common.h>

#include "Memory.h"

namespace rafi { namespace mem {

Memory::Memory()
{
    m_pBody = new char[MemorySize];
    std::memset(m_pBody, 0, MemorySize);
}

Memory::~Memory()
{
    delete[] m_pBody;
}

void Memory::LoadFile(const char* path, int offset)
{
    if (!(0 <= offset && offset < MemorySize))
    {
        ABORT();
    }

    std::ifstream f;
    f.open(path, std::fstream::binary | std::fstream::in);
    if (!f.is_open())
    {
        ABORT();
    }
    f.read(&m_pBody[offset], MemorySize - offset);
    f.close();
}

void Memory::Copy(void* pOut, size_t size) const
{
    if (size > MemorySize)
    {
        ABORT();
    }

    std::memcpy(pOut, m_pBody, size);
}

int32_t Memory::GetInt32(int address) const
{
    assert(0 <= address && address + sizeof(int32_t) <= MemorySize);
    return *reinterpret_cast<int32_t*>(&m_pBody[address]);
}

int8_t Memory::GetInt8(int address)
{
    assert(0 <= address && address + sizeof(int8_t) <= MemorySize);
    return *reinterpret_cast<int8_t*>(&m_pBody[address]);
}

void Memory::SetInt8(int address, int8_t value)
{
    assert(0 <= address && address + sizeof(int8_t) <= MemorySize);
    *reinterpret_cast<int8_t*>(&m_pBody[address]) = value;
}

int16_t Memory::GetInt16(int address)
{
    assert(0 <= address && address + sizeof(int16_t) <= MemorySize);
    return *reinterpret_cast<int16_t*>(&m_pBody[address]);
}

void Memory::SetInt16(int address, int16_t value)
{
    assert(0 <= address && address + sizeof(int16_t) <= MemorySize);
    *reinterpret_cast<int16_t*>(&m_pBody[address]) = value;
}

int32_t Memory::GetInt32(int address)
{
    assert(0 <= address && address + sizeof(int32_t) <= MemorySize);
    return *reinterpret_cast<int32_t*>(&m_pBody[address]);
}

void Memory::SetInt32(int address, int32_t value)
{
    assert(0 <= address && address + sizeof(int32_t) <= MemorySize);
    *reinterpret_cast<int32_t*>(&m_pBody[address]) = value;
}

}}
