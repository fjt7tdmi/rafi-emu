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
    explicit System(uint32_t pc, int ramSize);

    // Setup
    void LoadFileToMemory(const char* path, PhysicalAddress address);

    // Process
    void ProcessOneCycle();

    // for Dump
    int GetCsrCount() const;
    int GetRamSize() const;
    int GetMemoryAccessEventCount() const;

    uint32_t GetHostIoValue() const;

    void CopyCsr(void* pOut, size_t size) const;
    void CopyIntReg(void* pOut, size_t size) const;
    void CopyFpReg(void* pOut, size_t size) const;
    void CopyRam(void* pOut, size_t size) const;
    void CopyCsrReadEvent(CsrReadEvent* pOut) const;
    void CopyCsrWriteEvent(CsrWriteEvent* pOut) const;
    void CopyOpEvent(OpEvent* pOut) const;
    void CopyTrapEvent(TrapEvent* pOut) const;
    void CopyMemoryAccessEvent(MemoryAccessEvent* pOut, int index) const;

    bool IsCsrReadEventExist() const;
    bool IsCsrWriteEventExist() const;
    bool IsOpEventExist() const;
    bool IsTrapEventExist() const;

    void PrintStatus() const;

private:
    bus::Bus m_Bus;
    mem::Ram m_Ram;
    mem::Rom m_Rom;
    io::Uart m_Uart;
    io::Timer m_Timer;

    io::IoInterruptSource m_ExternalInterruptSource;
    io::IoInterruptSource m_TimerInterruptSource;

    cpu::Processor m_Processor;
};

}}
