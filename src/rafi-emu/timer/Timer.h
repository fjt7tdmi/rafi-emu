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

#include <vector>

#include <rafi/BasicTypes.h>

#include "../io/IIo.h"

#include "TimerTypes.h"

namespace rafi { namespace emu { namespace timer {

class Timer : public io::IIo
{
public:
    virtual int8_t GetInt8(int address) override;
    virtual void SetInt8(int address, int8_t value) override;

    virtual int16_t GetInt16(int address) override;
    virtual void SetInt16(int address, int16_t value) override;

    virtual int32_t GetInt32(int address) override;
    virtual void SetInt32(int address, int32_t value) override;

    virtual int GetSize() const override
    {
        return RegSize;
    }

    virtual bool IsInterruptRequested() const override;

    void ProcessCycle();

private:
    static const int RegSize = 16;

    uint64_t m_Time;
    uint64_t m_TimeCmp;
};

}}}
