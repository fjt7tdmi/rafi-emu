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

#include <cstring>

#include <rafi/emu.h>

#include "FpRegFile.h"

namespace rafi { namespace emu { namespace cpu {

FpRegFile::FpRegFile()
{
    std::memset(m_Entries, 0, sizeof(m_Entries));
}

void FpRegFile::Copy(void* pOut, size_t size) const
{
    assert(size == sizeof(m_Entries));

    std::memcpy(pOut, m_Entries, size);
}

uint32_t FpRegFile::ReadUInt32(int regId) const
{
    RAFI_EMU_CHECK_RANGE(0, regId, RegCount);

    return m_Entries[regId].u32.value;
}

uint64_t FpRegFile::ReadUInt64(int regId) const
{
    RAFI_EMU_CHECK_RANGE(0, regId, RegCount);

    return m_Entries[regId].u64.value;
}

float FpRegFile::ReadFloat(int regId) const
{
    RAFI_EMU_CHECK_RANGE(0, regId, RegCount);

    return m_Entries[regId].f.value;
}

double FpRegFile::ReadDouble(int regId) const
{
    RAFI_EMU_CHECK_RANGE(0, regId, RegCount);

    return m_Entries[regId].d.value;
}

void FpRegFile::WriteUInt32(int regId, uint32_t value)
{
    RAFI_EMU_CHECK_RANGE(0, regId, RegCount);

    m_Entries[regId].u32.value = value;
    m_Entries[regId].u32.zero = 0;
}

void FpRegFile::WriteUInt64(int regId, uint64_t value)
{
    RAFI_EMU_CHECK_RANGE(0, regId, RegCount);

    m_Entries[regId].u64.value = value;
}

void FpRegFile::WriteFloat(int regId, float value)
{
    RAFI_EMU_CHECK_RANGE(0, regId, RegCount);

    m_Entries[regId].f.value = value;
    m_Entries[regId].f.zero = 0;
}

void FpRegFile::WriteDouble(int regId, double value)
{
    RAFI_EMU_CHECK_RANGE(0, regId, RegCount);

    m_Entries[regId].d.value = value;
}

}}}
