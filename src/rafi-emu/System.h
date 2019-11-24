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

#include <rafi/emu.h>

#include "io/IoInterruptSource.h"
#include "io/Clint.h"
#include "io/Plic.h"
#include "io/Uart.h"
#include "io/Uart16550.h"
#include "io/Timer.h"
#include "io/VirtIo.h"

#include "mem/Ram.h"
#include "mem/Rom.h"

#include "bus/Bus.h"

#include "cpu/Processor.h"

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
    void ProcessCycle();

    // for gdbserver
    bool IsValidMemory(paddr_t addr, size_t size) const;
    void ReadMemory(void* pOutBuffer, size_t bufferSize, paddr_t addr);
    void WriteMemory(const void* pBuffer, size_t bufferSize, paddr_t addr);

    // for Dump
    int GetCsrCount() const;
    size_t GetRamSize() const;
    size_t GetMemoryAccessEventCount() const;

    uint32_t GetHostIoValue() const;
    vaddr_t GetPc() const;

    void CopyIntReg(trace::NodeIntReg32* pOut) const;
    void CopyIntReg(trace::NodeIntReg64* pOut) const;
    void CopyCsr(trace::Csr32Node* pOutNodes, int nodeCount) const;
    void CopyCsr(trace::Csr64Node* pOutNodes, int nodeCount) const;
    void CopyFpReg(void* pOut, size_t size) const;
    void CopyOpEvent(OpEvent* pOut) const;
    void CopyTrapEvent(TrapEvent* pOut) const;
    void CopyMemoryAccessEvent(MemoryAccessEvent* pOut, int index) const;
    
    bool IsOpEventExist() const;
    bool IsTrapEventExist() const;

    void PrintStatus() const;

private:
    static const paddr_t AddrRom = 0x00001000;
    static const paddr_t AddrRam = 0x80000000;

    // E31 compatible IOs
    static const paddr_t AddrClint      = 0x02000000;
    static const paddr_t AddrPlic       = 0x0c000000;
    static const paddr_t AddrUart16550  = 0x10000000;
    static const paddr_t AddrVirtIo1    = 0x10001000;
    static const paddr_t AddrVirtIo2    = 0x10002000;
    static const paddr_t AddrVirtIo3    = 0x10003000;
    static const paddr_t AddrVirtIo4    = 0x10004000;
    static const paddr_t AddrVirtIo5    = 0x10005000;
    static const paddr_t AddrVirtIo6    = 0x10006000;
    static const paddr_t AddrVirtIo7    = 0x10007000;
    static const paddr_t AddrVirtIo8    = 0x10008000;

    // IOs for zephyr
    static const paddr_t AddrUart   = 0x40002000;
    static const paddr_t AddrTimer  = 0x40000000;

    bus::Bus m_Bus;
    mem::Ram m_Ram;
    mem::Rom m_Rom;

    // E31 compatible IOs
    io::Clint m_Clint;
    io::Plic m_Plic;
    io::Uart16550 m_Uart16550;
    io::VirtIo m_VirtIo1;
    io::VirtIo m_VirtIo2;
    io::VirtIo m_VirtIo3;
    io::VirtIo m_VirtIo4;
    io::VirtIo m_VirtIo5;
    io::VirtIo m_VirtIo6;
    io::VirtIo m_VirtIo7;
    io::VirtIo m_VirtIo8;

    // IOs for zephyr
    io::Uart m_Uart;
    io::Timer m_Timer;

    io::IoInterruptSource m_ExternalInterruptSource;
    io::IoInterruptSource m_TimerInterruptSource;

    cpu::Processor m_Processor;

    uint32_t m_HostIoAddress{0};
};

}}
