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

#include "Rom.h"

namespace rafi { namespace mem {

Rom::Rom()
{
    m_pBody = new char[Capacity];
    std::memset(m_pBody, 0, Capacity);
}

Rom::~Rom()
{
    delete[] m_pBody;
}

void Rom::LoadFile(const char* path, int offset)
{
    if (!(0 <= offset && offset < Capacity))
    {
        ABORT();
    }

    std::ifstream f;
    f.open(path, std::fstream::binary | std::fstream::in);
    if (!f.is_open())
    {
        ABORT();
    }
    f.read(&m_pBody[offset], Capacity - offset);
    f.close();
}

int8_t Rom::GetInt8(int address) const
{
    assert(0 <= address && address + sizeof(int8_t) <= Capacity);
    return *reinterpret_cast<int8_t*>(&m_pBody[address]);
}

void Rom::SetInt8(int address, int8_t value)
{
    (void)address;
    (void)value;
    ABORT();
}

int16_t Rom::GetInt16(int address) const
{
    assert(0 <= address && address + sizeof(int16_t) <= Capacity);
    return *reinterpret_cast<int16_t*>(&m_pBody[address]);
}

void Rom::SetInt16(int address, int16_t value)
{
    (void)address;
    (void)value;
    ABORT();
}

int32_t Rom::GetInt32(int address) const
{
    assert(0 <= address && address + sizeof(int32_t) <= Capacity);
    return *reinterpret_cast<int32_t*>(&m_pBody[address]);
}

void Rom::SetInt32(int address, int32_t value)
{
    (void)address;
    (void)value;
    ABORT();
}

}}
