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

namespace rafi { namespace cpu {

class IntRegFile
{
public:
    static const int IntRegCount = 32;

    IntRegFile();

    void Copy(void* pOut, size_t size) const;

	int32_t Read(int regId) const;

    void Write(int regId, int32_t value);

private:
	int32_t m_Body[IntRegCount];
};

}}
