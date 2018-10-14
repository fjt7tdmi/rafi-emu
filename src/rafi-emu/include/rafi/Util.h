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

#include <cstdio>
#include <cstdlib>
#include <cstdint>

#define ABORT() do { fprintf(stderr, "ABORT (%s:%d)\n", __FILE__, __LINE__); std::abort(); } while (0)

inline int32_t GetHigh32(uint64_t value)
{
    return static_cast<int32_t>(value >> 32);
}

inline int32_t GetHigh32(int64_t value)
{
    return GetHigh32(static_cast<uint64_t>(value));
}

inline int32_t GetLow32(uint64_t value)
{
    return static_cast<int32_t>(value & 0xffffffff);
}

inline int32_t GetLow32(int64_t value)
{
    return GetLow32(static_cast<uint64_t>(value));
}

inline void SetHigh32(uint64_t* pOut, int32_t value)
{
    (*pOut) &= 0x00000000ffffffffLL;
    (*pOut) |= (static_cast<int64_t>(value) << 32);
}

inline void SetHigh32(int64_t* pOut, int32_t value)
{
    SetHigh32(reinterpret_cast<uint64_t*>(pOut), value);
}

inline void SetLow32(uint64_t* pOut, int32_t value)
{
    (*pOut) &= 0xffffffff00000000LL;
    (*pOut) |= value;
}

inline void SetLow32(int64_t* pOut, int32_t value)
{
    SetLow32(reinterpret_cast<uint64_t*>(pOut), value);
}
