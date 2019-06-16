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

#include <cassert>
#include <cstdint>

#include <rafi/emu.h>

namespace rafi { namespace emu { namespace cpu {

class IntRegFile
{
public:
    IntRegFile();

    void Copy(trace::NodeIntReg32* pOut) const;
    void Copy(trace::NodeIntReg64* pOut) const;

	int32_t ReadInt32(int regId) const;
	int64_t ReadInt64(int regId) const;
	uint32_t ReadUInt32(int regId) const;
	uint64_t ReadUInt64(int regId) const;

    void WriteInt32(int regId, int32_t value);
    void WriteInt64(int regId, int64_t value);
    void WriteUInt32(int regId, uint32_t value);
    void WriteUInt64(int regId, uint64_t value);

private:
    union Entry
    {
        struct
        {
            int32_t value;
            int32_t invalid;
        } s32;
        struct
        {
            int64_t value;
        } s64;
        struct
        {
            uint32_t value;
            uint32_t invalid;
        } u32;
        struct
        {
            uint64_t value;
        } u64;
    };

	Entry m_Entries[IntRegCount];
};

}}}
