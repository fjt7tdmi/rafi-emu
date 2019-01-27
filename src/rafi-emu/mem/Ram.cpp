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

#include "Ram.h"

namespace rafi { namespace emu { namespace mem {

Ram::Ram(int capacity)
    : m_Capacity(capacity)
{
    m_pBody = new char[capacity];
    std::memset(m_pBody, 0, capacity);
}

Ram::~Ram()
{
    delete[] m_pBody;
}

int Ram::GetCapacity() const
{
    return m_Capacity;
}

void Ram::LoadFile(const char* path, int offset)
{
    if (!(0 <= offset && offset < m_Capacity))
    {
        ABORT();
    }

    std::ifstream f;
    f.open(path, std::fstream::binary | std::fstream::in);
    if (!f.is_open())
    {
        ABORT();
    }
    f.read(&m_pBody[offset], m_Capacity - offset);
    f.close();
}

void Ram::Copy(void* pOut, size_t size) const
{
    if (size > static_cast<size_t>(m_Capacity))
    {
        ABORT();
    }

    std::memcpy(pOut, m_pBody, size);
}

void Ram::Read(void* pOutBuffer, size_t size, uint64_t address) const
{
    assert(0 <= address && address + size <= GetCapacity());

    std::memcpy(pOutBuffer, &m_pBody[address], size);
}

void Ram::Write(const void* pBuffer, size_t size, uint64_t address)
{
    assert(0 <= address && address + size <= GetCapacity());

    std::memcpy(&m_pBody[address], pBuffer, size);
}

}}}
