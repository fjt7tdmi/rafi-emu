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

namespace rafi { namespace io {

class IIo
{
public:
    virtual int8_t GetInt8(int address) = 0;
    virtual void SetInt8(int address, int8_t value) = 0;

    virtual int16_t GetInt16(int address) = 0;
    virtual void SetInt16(int address, int16_t value) = 0;

    virtual int32_t GetInt32(int address) = 0;
    virtual void SetInt32(int address, int32_t value) = 0;

    virtual int GetSize() const = 0;

    virtual bool IsInterruptRequested() const = 0;
};

}}