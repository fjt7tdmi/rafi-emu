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

#include "io/IoInterruptSource.h"
#include "io/Uart.h"
#include "io/Uart16550.h"
#include "io/Timer.h"
#include "mem/Ram.h"
#include "mem/Rom.h"
#include "bus/Bus.h"
#include "cpu/Processor.h"

#include <rafi/emu.h>

namespace rafi { namespace emu {

class System
{
public:
    System(XLEN xlen, vaddr_t pc, size_t ramSize);

    // Setup
    void LoadFileToMemory(const char* path, paddr_t address);
    void SetDtbAddress(vaddr_t address);
    void SetHostIoAddress(vaddr_t address);

    // Process
    void ProcessOneCycle();

    // for Dump
    int GetCsrCount() const;
    size_t GetRamSize() const;
    size_t GetMemoryAccessEventCount() const;

    uint32_t GetHostIoValue() const;

    void CopyIntReg(trace::IntReg32Node* pOut) const;
    void CopyIntReg(trace::IntReg64Node* pOut) const;
    void CopyCsr(trace::Csr32Node* pOutNodes, int nodeCount) const;
    void CopyCsr(trace::Csr64Node* pOutNodes, int nodeCount) const;
    void CopyFpReg(void* pOut, size_t size) const;
    void CopyRam(void* pOut, size_t size) const;
    void CopyOpEvent(OpEvent* pOut) const;
    void CopyTrapEvent(TrapEvent* pOut) const;
    void CopyMemoryAccessEvent(MemoryAccessEvent* pOut, int index) const;

    bool IsOpEventExist() const;
    bool IsTrapEventExist() const;

    void PrintStatus() const;

private:
    static const paddr_t AddrRom = 0x00001000;
    static const paddr_t AddrRam = 0x80000000;

    static const paddr_t AddrUart16550 = 0x10000000;
    static const paddr_t AddrUart = 0x40002000;
    static const paddr_t AddrTimer = 0x40000000;

    bus::Bus m_Bus;
    mem::Ram m_Ram;
    mem::Rom m_Rom;
    io::Uart m_Uart;
    io::Uart16550 m_Uart16550;
    io::Timer m_Timer;

    io::IoInterruptSource m_ExternalInterruptSource;
    io::IoInterruptSource m_TimerInterruptSource;

    cpu::Processor m_Processor;

    uint32_t m_HostIoAddress{0};
};

}}
