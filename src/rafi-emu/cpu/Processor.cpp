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

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>

#include <rvtrace/common.h>

#include "Processor.h"

using namespace std;
using namespace rvtrace;

namespace rafi { namespace emu { namespace cpu {

void Processor::RegisterExternalInterruptSource(IInterruptSource* pInterruptSource)
{
    m_InterruptController.RegisterExternalInterruptSource(pInterruptSource);
}

void Processor::RegisterTimerInterruptSource(IInterruptSource* pInterruptSource)
{
    m_InterruptController.RegisterTimerInterruptSource(pInterruptSource);
}

void Processor::SetIntReg(int regId, int32_t regValue)
{
    m_IntRegFile.WriteInt32(regId, regValue);
}

void Processor::ProcessOneCycle()
{
    ClearOpEvent();
    m_TrapProcessor.ClearEvent();
    m_MemAccessUnit.ClearEvent();

    const auto privilegeLevel = m_Csr.GetPrivilegeLevel();
    const auto pc = m_Csr.GetProgramCounter();

    // Check interrupt
    m_InterruptController.Update();

    if (m_InterruptController.IsRequested())
    {
        m_Csr.SetHaltFlag(false);

        const auto interruptType = m_InterruptController.GetInterruptType();

        m_TrapProcessor.ProcessInterrupt(interruptType, pc);

        SetOpEvent(pc, privilegeLevel);
        return;
    }

    if (m_Csr.GetHaltFlag())
    {
        return;
    }

    // Fetch
    PhysicalAddress physicalPc = InvalidValue;

    const auto fetchTrap = m_MemAccessUnit.CheckTrap(MemoryAccessType::Instruction, pc, pc);
    if (fetchTrap)
    {
        m_TrapProcessor.ProcessException(fetchTrap.value());

        SetOpEvent(pc, privilegeLevel);
        return;
    }

    const auto insn = m_MemAccessUnit.FetchInt32(&physicalPc, pc);

    // Decode
    const auto op = m_Decoder.Decode(insn);
    if (op.opCode == OpCode::unknown)
    {
        const auto decodeTrap = MakeIllegalInstructionException(pc, insn);

        m_TrapProcessor.ProcessException(decodeTrap);

        SetOpEvent(pc, physicalPc, insn, op.opCode, privilegeLevel);
        return;
    }

    // Execute
    const auto preExecuteTrap = m_Executor.PreCheckTrap(op, pc, insn);
    if (preExecuteTrap)
    {
        m_TrapProcessor.ProcessException(preExecuteTrap.value());

        SetOpEvent(pc, physicalPc, insn, op.opCode, privilegeLevel);
        return;
    }

    m_Csr.SetProgramCounter(pc + 4);

    m_Executor.ProcessOp(op, pc);

    auto postExecuteTrap = m_Executor.PostCheckTrap(op, pc);
    if (postExecuteTrap)
    {
        m_TrapProcessor.ProcessException(postExecuteTrap.value());

        SetOpEvent(pc, physicalPc, insn, op.opCode, privilegeLevel);
        return;
    }

    SetOpEvent(pc, physicalPc, insn, op.opCode, privilegeLevel);
}

int Processor::GetCsrCount() const
{
    return m_Csr.GetRegisterCount();
}

void Processor::CopyCsr(void* pOut, size_t size) const
{
    m_Csr.Copy(pOut, size);
}

void Processor::CopyIntReg(void* pOut, size_t size) const
{
    m_IntRegFile.Copy(pOut, size);
}

void Processor::CopyFpReg(void* pOut, size_t size) const
{
    m_FpRegFile.Copy(pOut, size);
}

void Processor::CopyCsrReadEvent(CsrReadEvent* pOut) const
{
    m_CsrAccessor.CopyReadEvent(pOut);
}

void Processor::CopyCsrWriteEvent(CsrWriteEvent* pOut) const
{
    m_CsrAccessor.CopyWriteEvent(pOut);
}

void Processor::CopyOpEvent(OpEvent* pOut) const
{
    std::memcpy(pOut, &m_OpEvent, sizeof(*pOut));
}

void Processor::CopyMemoryAccessEvent(MemoryAccessEvent* pOut) const
{
    m_MemAccessUnit.CopyEvent(pOut);
}

void Processor::CopyTrapEvent(TrapEvent* pOut) const
{
    m_TrapProcessor.CopyTrapEvent(pOut);
}

bool Processor::IsCsrReadEventExist() const
{
    return m_CsrAccessor.IsReadEventExist();
}

bool Processor::IsCsrWriteEventExist() const
{
    return m_CsrAccessor.IsWriteEventExist();
}

bool Processor::IsOpEventExist() const
{
    return m_OpEventValid;
}

bool Processor::IsMemoryAccessEventExist() const
{
    return m_MemAccessUnit.IsEventExist();
}

bool Processor::IsTrapEventExist() const
{
    return m_TrapProcessor.IsTrapEventExist();
}

void Processor::PrintStatus() const
{
    printf("    OpCount: %d (0x%x)\n", m_OpCount, m_OpCount);
    printf("    PC:      0x%x\n", m_Csr.GetProgramCounter());
}

void Processor::ClearOpEvent()
{
    m_OpEventValid = false;
}

void Processor::SetOpEvent(int32_t virtualPc, PrivilegeLevel privilegeLevel)
{
    SetOpEvent(virtualPc, InvalidValue, InvalidValue, OpCode::unknown, privilegeLevel);
}

void Processor::SetOpEvent(int32_t virtualPc, PhysicalAddress physicalPc, int32_t insn, OpCode opCode, PrivilegeLevel privilegeLevel)
{
    m_OpEvent.insn = insn;
    m_OpEvent.opCode = opCode;
    m_OpEvent.opId = m_OpCount;
    m_OpEvent.virtualPc = virtualPc;
    m_OpEvent.physicalPc = physicalPc;
    m_OpEvent.privilegeLevel = privilegeLevel;

    m_OpEventValid = true;

    m_OpCount++;
}

}}}
