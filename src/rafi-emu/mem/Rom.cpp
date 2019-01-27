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

namespace rafi { namespace emu { namespace mem {

Rom::Rom()
{
    m_pBody = new char[Capacity];
    std::memset(m_pBody, 0, Capacity);
}

Rom::~Rom()
{
    delete[] m_pBody;
}

int Rom::GetCapacity() const
{
    return Capacity;
}

void Rom::LoadFile(const char* path, int offset)
{
    RAFI_EMU_CHECK_RANGE(0, offset, GetCapacity());

    std::ifstream f;
    f.open(path, std::fstream::binary | std::fstream::in);
    if (!f.is_open())
    {
        RAFI_EMU_ERROR("Failed to open file: %s\n", path);
    }
    f.read(&m_pBody[offset], Capacity - offset);
    f.close();
}

void Rom::Read(void* pOutBuffer, size_t size, uint64_t address) const
{
    RAFI_EMU_CHECK_ACCESS(address, size, GetCapacity());

    std::memcpy(pOutBuffer, &m_pBody[address], size);
}

void Rom::Write(const void* pBuffer, size_t size, uint64_t address)
{
    static_cast<void>(pBuffer);
    static_cast<void>(size);
    static_cast<void>(address);

    RAFI_EMU_ERROR("Rom does not support write operation.\n");
}

}}}
