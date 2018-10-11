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

#include "uart/Uart.h"
#include "mem/Memory.h"
#include "bus/Bus.h"
#include "cpu/Processor.h"

#include <rafi/Event.h>

namespace rafi {

class System
{
public:
    explicit System(int32_t initialPc);

    // Setup
    void LoadFileToMemory(const char* path, PhysicalAddress address);
    void SetupDtbAddress(int32_t address);

    // Process
    void ProcessOneCycle();

    // for Dump
    int GetCsrSize() const;
    int GetMemorySize() const;

    int32_t GetHostIoValue() const;

    void CopyCsr(void* pOut, size_t size) const;
    void CopyIntRegs(void* pOut, size_t size) const;
    void CopyMemory(void* pOut, size_t size) const;

    void CopyCsrReadEvent(CsrReadEvent* pOut) const;
    void CopyCsrWriteEvent(CsrWriteEvent* pOut) const;
    void CopyMemoryAccessEvent(MemoryAccessEvent* pOut) const;
    void CopyOpEvent(OpEvent* pOut) const;
    void CopyTrapEvent(TrapEvent* pOut) const;

    bool IsCsrReadEventExist() const;
    bool IsCsrWriteEventExist() const;
    bool IsMemoryAccessEventExist() const;
    bool IsOpEventExist() const;
    bool IsTrapEventExist() const;

private:
    // dtb address register is a1(x11)
    const int DtbAddressRegId = 11;

    uart::Uart m_Uart;
    mem::Memory m_Memory;
    bus::Bus m_Bus;
    cpu::Processor m_Processor;
};

}
