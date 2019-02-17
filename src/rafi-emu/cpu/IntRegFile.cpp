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

#include "IntRegFile.h"

namespace rafi { namespace emu { namespace cpu {

IntRegFile::IntRegFile()
{
    std::memset(m_Entries, 0, sizeof(m_Entries));
}

void IntRegFile::Copy(trace::IntReg32Node* pOut) const
{
    for (int i = 0; i < IntRegCount; i++)
    {
        pOut->regs[i] = m_Entries[i].u32.value;
    }
}

void IntRegFile::Copy(trace::IntReg64Node* pOut) const
{
    for (int i = 0; i < IntRegCount; i++)
    {
        pOut->regs[i] = m_Entries[i].u64.value;
    }
}

int32_t IntRegFile::ReadInt32(int regId) const
{
    RAFI_EMU_CHECK_RANGE(0, regId, IntRegCount);
    
    return m_Entries[regId].s32.value;
}

uint32_t IntRegFile::ReadUInt32(int regId) const
{
    RAFI_EMU_CHECK_RANGE(0, regId, IntRegCount);

    return m_Entries[regId].u32.value;
}

void IntRegFile::WriteInt32(int regId, int32_t value)
{
    RAFI_EMU_CHECK_RANGE(0, regId, IntRegCount);

    if (regId != 0)
    {
        m_Entries[regId].s32.value = value;
    }
}

void IntRegFile::WriteUInt32(int regId, uint32_t value)
{
    RAFI_EMU_CHECK_RANGE(0, regId, IntRegCount);
    
    if (regId != 0)
    {
        m_Entries[regId].u32.value = value;
    }
}

}}}
