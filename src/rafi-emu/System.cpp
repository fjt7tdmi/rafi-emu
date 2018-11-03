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

#include "System.h"
#include "MemoryMap.h"

namespace rafi { namespace emu {

System::System(int32_t initialPc)
    : m_Bus()
    , m_Ram()
    , m_Uart()
    , m_Timer()
    , m_ExternalInterruptSource(&m_Uart)
    , m_TimerInterruptSource(&m_Timer)
    , m_Processor(&m_Bus, initialPc)
{
    m_Bus.RegisterMemory(&m_Ram, RamAddr, m_Ram.Capacity);
    m_Bus.RegisterMemory(&m_Rom, RomAddr, m_Rom.Capacity);
    m_Bus.RegisterIo(&m_Uart, UartAddr, m_Uart.GetSize());
    m_Bus.RegisterIo(&m_Timer, TimerAddr, m_Timer.GetSize());

    m_Processor.RegisterExternalInterruptSource(&m_ExternalInterruptSource);
    m_Processor.RegisterTimerInterruptSource(&m_TimerInterruptSource);
}

void System::SetupDtbAddress(int32_t address)
{
    m_Processor.SetIntReg(DtbAddressRegId, address);
}

void System::LoadFileToMemory(const char* path, PhysicalAddress address)
{
    auto location = m_Bus.ConvertToMemoryLocation(address);
    location.pMemory->LoadFile(path, location.offset);
}

void System::ProcessOneCycle()
{
    m_Uart.ProcessCycle();
    m_Timer.ProcessCycle();
    m_Processor.ProcessOneCycle();
}

int System::GetCsrCount() const
{
    return m_Processor.GetCsrCount();
}

int System::GetRamSize() const
{
    return m_Ram.Capacity;
}

int32_t System::GetHostIoValue() const
{
    auto location = m_Bus.ConvertToMemoryLocation(HostIoAddr);
    return m_Ram.GetInt32(location.offset);
}

void System::CopyCsr(void* pOut, size_t size) const
{
    m_Processor.CopyCsr(pOut, size);
}

void System::CopyIntRegs(void* pOut, size_t size) const
{
    m_Processor.CopyIntRegs(pOut, size);
}

void System::CopyRam(void* pOut, size_t size) const
{
    m_Ram.Copy(pOut, size);
}

void System::CopyCsrReadEvent(CsrReadEvent* pOut) const
{
    return m_Processor.CopyCsrReadEvent(pOut);
}

void System::CopyCsrWriteEvent(CsrWriteEvent* pOut) const
{
    return m_Processor.CopyCsrWriteEvent(pOut);
}

void System::CopyMemoryAccessEvent(MemoryAccessEvent* pOut) const
{
    return m_Processor.CopyMemoryAccessEvent(pOut);
}

void System::CopyOpEvent(OpEvent* pOut) const
{
    return m_Processor.CopyOpEvent(pOut);
}

void System::CopyTrapEvent(TrapEvent* pOut) const
{
    return m_Processor.CopyTrapEvent(pOut);
}

bool System::IsCsrReadEventExist() const
{
    return m_Processor.IsCsrReadEventExist();
}

bool System::IsCsrWriteEventExist() const
{
    return m_Processor.IsCsrWriteEventExist();
}

bool System::IsMemoryAccessEventExist() const
{
    return m_Processor.IsMemoryAccessEventExist();
}

bool System::IsOpEventExist() const
{
    return m_Processor.IsOpEventExist();
}

bool System::IsTrapEventExist() const
{
    return m_Processor.IsTrapEventExist();
}

}}
