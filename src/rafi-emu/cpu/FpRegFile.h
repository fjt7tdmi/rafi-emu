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

#include <rafi/Exception.h>

namespace rafi { namespace emu { namespace cpu {

class FpRegFile
{
public:
    static const int RegCount = 32;

    FpRegFile();

    void Copy(void* pOut, size_t size) const;

    uint32_t ReadUInt32(int regId) const;
    uint64_t ReadUInt64(int regId) const;
	float ReadFloat(int regId) const;
	double ReadDouble(int regId) const;

    void WriteUInt32(int regId, uint32_t value);
    void WriteUInt64(int regId, uint64_t value);
    void WriteFloat(int regId, float value);
    void WriteDouble(int regId, double value);

private:
	uint64_t m_Body[RegCount];
};

}}}
