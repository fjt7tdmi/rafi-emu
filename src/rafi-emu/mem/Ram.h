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

#include "IMemory.h"

namespace rafi { namespace mem {

class Ram : public IMemory
{
    Ram(const Ram&) = delete;
    Ram(Ram&&) = delete;
    Ram& operator=(const Ram&) = delete;
    Ram& operator=(Ram&&) = delete;

public:
    Ram();
    ~Ram();

    void LoadFile(const char* path, int offset = 0);

    void Copy(void* pOut, size_t size) const;

    virtual int8_t GetInt8(int address) const override;
    virtual void SetInt8(int address, int8_t value) override;

    virtual int16_t GetInt16(int address) const override;
    virtual void SetInt16(int address, int16_t value) override;

    virtual int32_t GetInt32(int address) const override;
    virtual void SetInt32(int address, int32_t value) override;

    // Constants
    static const int Capacity = 64 * 1024 * 1024;

private:
	char* m_pBody;
};

}}
