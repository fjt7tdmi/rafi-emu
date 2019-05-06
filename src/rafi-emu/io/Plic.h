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

#include "../cpu/Processor.h"
#include "../io/IIo.h"

namespace rafi { namespace emu { namespace io {

class Plic : public IIo
{
public:
    virtual void Read(void* pOutBuffer, size_t size, uint64_t address) override;
    virtual void Write(const void* pBuffer, size_t size, uint64_t address) override;
    virtual int GetSize() const override;
    virtual bool IsInterruptRequested() const override;

private:
    uint32_t ReadUInt32(uint64_t address);
    void WriteUInt32(uint64_t address, uint32_t value);

    static const int InterruptCount = 128;
    static const int PendingArraySize = InterruptCount / 32;
    static const int EnableArraySize = InterruptCount / 32;

    static const uint32_t PriorityMask = 0x7;

    static const int RegisterSpaceSize = 0x0400'0000;

    // Register address
    static const uint64_t ADDR_PRIORITY_BEGIN   = 0x00'0000;
    static const uint64_t ADDR_PRIORITY_END     = 0x00'0200;
    static const uint64_t ADDR_PENDING_BEGIN    = 0x00'1000;
    static const uint64_t ADDR_PENDING_END      = 0x00'1010;
    static const uint64_t ADDR_ENABLE_M_BEGIN   = 0x00'2000;
    static const uint64_t ADDR_ENABLE_M_END     = 0x00'2010;
    static const uint64_t ADDR_ENABLE_S_BEGIN   = 0x00'2080;
    static const uint64_t ADDR_ENABLE_S_END     = 0x00'2090;
    static const uint64_t ADDR_THRESHOLD_M      = 0x20'0000;
    static const uint64_t ADDR_CLAIM_COMPLETE_M = 0x20'0004;
    static const uint64_t ADDR_THRESHOLD_S      = 0x20'1000;
    static const uint64_t ADDR_CLAIM_COMPLETE_S = 0x20'1004;

    cpu::Processor* m_pProcessor;

    uint32_t m_Priorities[InterruptCount];
    uint32_t m_Pendings[PendingArraySize];
    uint32_t m_MachineInterruptEnables[EnableArraySize];
    uint32_t m_SupervisorInterruptEnables[EnableArraySize];
    uint32_t m_MachineThreshold;
    uint32_t m_SupervisorThreshold;
};

}}}
