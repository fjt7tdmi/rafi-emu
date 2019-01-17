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

#include "FpRegFile.h"

namespace rafi { namespace emu { namespace cpu {

FpRegFile::FpRegFile()
{
    std::memset(m_Body, 0, sizeof(m_Body));
}

void FpRegFile::Copy(void* pOut, size_t size) const
{
    if (size > sizeof(m_Body))
    {
        abort();
    }
    std::memcpy(pOut, m_Body, size);
}

uint32_t FpRegFile::ReadUInt32(int regId) const
{
    CHECK_RANGE(0, regId, RegCount);

    auto p = reinterpret_cast<const uint32_t*>(&m_Body[regId]);
    return *p;
}

uint64_t FpRegFile::ReadUInt64(int regId) const
{
    CHECK_RANGE(0, regId, RegCount);

    auto p = reinterpret_cast<const uint64_t*>(&m_Body[regId]);
    return *p;
}

float FpRegFile::ReadFloat(int regId) const
{
    CHECK_RANGE(0, regId, RegCount);

    auto p = reinterpret_cast<const float*>(&m_Body[regId]);
    return *p;
}

double FpRegFile::ReadDouble(int regId) const
{
    CHECK_RANGE(0, regId, RegCount);

    auto p = reinterpret_cast<const double*>(&m_Body[regId]);
    return *p;
}

void FpRegFile::WriteUInt32(int regId, uint32_t value)
{
    CHECK_RANGE(0, regId, RegCount);
    m_Body[regId] = *reinterpret_cast<uint32_t*>(&value);
}

void FpRegFile::WriteUInt64(int regId, uint64_t value)
{
    CHECK_RANGE(0, regId, RegCount);
    m_Body[regId] = *reinterpret_cast<uint64_t*>(&value);
}

void FpRegFile::WriteFloat(int regId, float value)
{
    CHECK_RANGE(0, regId, RegCount);
    m_Body[regId] = *reinterpret_cast<uint32_t*>(&value);
}

void FpRegFile::WriteDouble(int regId, double value)
{
    CHECK_RANGE(0, regId, RegCount);
    m_Body[regId] = *reinterpret_cast<uint64_t*>(&value);
}

}}}
