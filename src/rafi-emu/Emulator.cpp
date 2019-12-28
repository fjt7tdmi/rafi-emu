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

Emulator::Emulator(XLEN xlen, vaddr_t pc, size_t ramSize)
    : m_System(xlen, pc, ramSize)
{
}

Emulator::~Emulator()
{
}

System* Emulator::GetSystem()
{
    return &m_System;
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

}}
