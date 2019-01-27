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

#include <rafi/emu.h>

#include "../io/IIo.h"

namespace rafi { namespace emu { namespace io {

class Timer : public IIo
{
public:
    virtual void Read(void* pOutBuffer, size_t size, uint64_t address) override;
    virtual void Write(const void* pBuffer, size_t size, uint64_t address) override;

    virtual int GetSize() const override
    {
        return RegSize;
    }

    virtual bool IsInterruptRequested() const override;

    void ProcessCycle();

private:
    static const int RegSize = 16;

    // Register address
    static const int Address_TimeLow = 0;
    static const int Address_TimeHigh = 4;
    static const int Address_TimeCmpLow = 8;
    static const int Address_TimeCmpHigh = 12;

    uint64_t m_Time;
    uint64_t m_TimeCmp;
};

}}}
