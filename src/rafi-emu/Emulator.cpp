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

#include <rafi/emu.h>

#include "Emulator.h"

namespace rafi { namespace emu {

Emulator::Emulator(CommandLineOption option)
    : m_Option(option)
    , m_System(option.GetXLEN(), option.GetPc(), option.GetRamSize())
    , m_Logger(option.GetXLEN(), option.GetTraceLoggerConfig(), &m_System)
{
    if (option.IsHostIoEnabled())
    {
        m_System.SetHostIoAddress(option.GetHostIoAddress());
    }

    m_System.SetDtbAddress(option.GetDtbAddress());
}

Emulator::~Emulator()
{
}

void Emulator::LoadFileToMemory(const char* path, paddr_t address)
{
    m_System.LoadFileToMemory(path, address);
}

void Emulator::PrintStatus() const
{
    m_System.PrintStatus();
}

int Emulator::GetCycle() const
{
    return m_Cycle;
}

void Emulator::Process(EmulationStop condition, int cycle)
{
    while (m_Cycle < cycle || cycle == CycleForever)
    {
        const bool dumpEnabled = m_Cycle >= m_Option.GetDumpSkipCycle();
        
        if (dumpEnabled)
        {
            m_Logger.BeginCycle(cycle, m_System.GetPc());
            m_Logger.RecordState();
        }

        if (IsStopConditionFilledPre(condition))
        {
            if (dumpEnabled)
            {
                m_Logger.EndCycle();
            }
            break;
        }

        ProcessCycle();

        if (dumpEnabled)
        {
            m_Logger.RecordEvent();
            m_Logger.EndCycle();
        }

        if (IsStopConditionFilledPost(condition))
        {
            break;
        }

        m_Cycle++;
    }
}

void Emulator::Process(EmulationStop condition)
{
    Process(condition, CycleForever);
}

void Emulator::ProcessCycle()
{
    m_System.ProcessCycle();
}

bool Emulator::IsValidMemory(paddr_t addr, size_t size) const
{
    return m_System.IsValidMemory(addr, size);
}

void Emulator::ReadMemory(void* pOutBuffer, size_t bufferSize, paddr_t addr)
{
    m_System.ReadMemory(pOutBuffer, bufferSize, addr);
}

void Emulator::WriteMemory(const void* pBuffer, size_t bufferSize, paddr_t addr)
{
    m_System.WriteMemory(pBuffer, bufferSize, addr);
}

vaddr_t Emulator::GetPc() const
{
    return m_System.GetPc();
}

void Emulator::CopyIntReg(trace::NodeIntReg32* pOut) const
{
    m_System.CopyIntReg(pOut);
}

void Emulator::CopyIntReg(trace::NodeIntReg64* pOut) const
{
    m_System.CopyIntReg(pOut);
}

bool Emulator::IsStopConditionFilledPre(EmulationStop condition)
{
    if (condition & EmulationStop_HostIo)
    {
        if (m_System.GetHostIoValue() != 0)
        {
            return true;
        }
    }

    return false;
}

bool Emulator::IsStopConditionFilledPost(EmulationStop condition)
{
    if (condition & EmulationStop_Breakpoint)
    {
        if (m_System.IsTrapEventExist())
        {
            TrapEvent trapEvent;
            m_System.CopyTrapEvent(&trapEvent);

            return trapEvent.trapType == TrapType::Exception && trapEvent.trapCause == static_cast<uint32_t>(ExceptionType::Breakpoint);
        }
    }

    return false;
}

}}
