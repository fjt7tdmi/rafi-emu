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

class Clint : public IIo
{
public:
    virtual void Read(void* pOutBuffer, size_t size, uint64_t address) override;
    virtual void Write(const void* pBuffer, size_t size, uint64_t address) override;
    virtual int GetSize() const override;
    virtual bool IsInterruptRequested() const override;

    void ProcessCycle();

    IInterruptSource* GetSoftwareInterruptSource();
    IInterruptSource* GetTimerInterruptSource();

private:
    class InterruptHolder : public IInterruptSource
    {
    public:
        virtual bool IsRequested() const
        {
            return m_Requested;
        }

        void SetRequested(bool requested)
        {
            m_Requested = requested;
        }

    private:
        bool m_Requested{ false };
    };

    void ReadMsip(void* pOutBuffer, size_t size);
    void WriteMsip(const void* pBuffer, size_t size);

    static const int RegisterSpaceSize = 0x10000;

    // Register address
    static const int ADDR_MSIP = 0x0;
    static const int ADDR_MTIME = 0x4000;
    static const int ADDR_MTIMECMP = 0xbff8;

    uint64_t m_Time{ 0 };
    uint64_t m_TimeCmp{ 0 };
    InterruptHolder m_SoftwareInterrupt;
    InterruptHolder m_TimerInterrupt;
};

}}}
