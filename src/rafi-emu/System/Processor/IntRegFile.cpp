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

#include "IntRegFile.h"

IntRegFile::IntRegFile()
{
    std::memset(m_Body, 0, sizeof(m_Body));
}

void IntRegFile::Copy(void* pOut, size_t size) const
{
    if (size > sizeof(m_Body))
    {
        abort();
    }
    std::memcpy(pOut, m_Body, size);
}

int32_t IntRegFile::Read(int regId) const
{
    CHECK_RANGE(0, regId, IntRegCount);
    return m_Body[regId];
}

void IntRegFile::Write(int regId, int32_t value)
{
    CHECK_RANGE(0, regId, IntRegCount);
    if (regId != 0)
    {
        m_Body[regId] = value;
    }
}
