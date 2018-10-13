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

#include <cstdint>
#include <cstring>

namespace rafi { namespace mem {

class Memory
{
    Memory(const Memory&) = delete;
    Memory(Memory&&) = delete;
    Memory& operator=(const Memory&) = delete;
    Memory& operator=(Memory&&) = delete;

public:
    Memory();
    ~Memory();

    void LoadFile(const char* path, int offset = 0);

    void Copy(void* pOut, size_t size) const;

    int8_t GetInt8(int address) const;
    void SetInt8(int address, int8_t value);

    int16_t GetInt16(int address) const;
    void SetInt16(int address, int16_t value);

    int32_t GetInt32(int address) const;
    void SetInt32(int address, int32_t value);

    virtual int GetSize() const
    {
        return MemorySize;
    }

    // Constants
    static const int MemoryAddrWidth = 26;
    static const int MemoryAddrMask = (1 << MemoryAddrWidth) - 1;
    static const int MemorySize = 1 << MemoryAddrWidth;

    // for Dump
    static const int32_t LineSize = 64;

private:
	char* m_pBody;
};

}}
