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

#include "Csr.h"
#include "Processor.h"
#include "ProcessorException.h"

using namespace std;
using namespace rvtrace;

void Processor::SetIntReg(int regId, int32_t regValue)
{
    m_IntRegFile.Write(regId, regValue);
}

void Processor::ProcessOneCycle()
{
    ClearOpEvent();
    m_TrapProcessor.ClearEvent();
    m_MemAccessUnit.ClearEvent();

    const auto pc = m_Csr.GetProgramCounter();

    // Check interrupt
    m_InterruptController.Update();

    if (m_InterruptController.IsRequested())
    {
        auto interruptType = m_InterruptController.GetInterruptType();

        m_TrapProcessor.ProcessInterrupt(interruptType, pc);

        SetOpEvent(pc);
        return;
    }

    // Fetch Op
    Op op { OpClass::RV32I, OpCode::unknown };

    int32_t insn = InvalidValue;
    PhysicalAddress physicalPc = InvalidValue;

    try
    {
        m_MemAccessUnit.CheckException(MemoryAccessType::Instruction, pc, pc);

        insn = m_MemAccessUnit.FetchInt32(&physicalPc, pc);

        m_Decoder.Decode(&op, insn);

        if (op.opCode == OpCode::unknown)
        {
            throw IllegalInstructionException(pc, insn);
        }

        m_Executor.PreCheckException(op, pc, insn);

        m_Csr.SetProgramCounter(pc + 4);

        m_Executor.ProcessOp(op, pc);

        m_Executor.PostCheckException(op, pc);
    }
    catch (const ProcessorException& e)
    {
        m_TrapProcessor.ProcessException(e);
    }

    // set event for Dump
    SetOpEvent(pc, physicalPc, insn, op.opCode);
}

int Processor::GetCsrSize() const
{
    return m_Csr.GetRegisterFileSize();
}

void Processor::CopyCsr(void* pOut, size_t size) const
{
    m_Csr.CopyRegisterFile(pOut, size);
}

void Processor::CopyIntRegs(void* pOut, size_t size) const
{
    m_IntRegFile.Copy(pOut, size);
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

void Processor::ClearOpEvent()
{
    m_OpEventValid = false;
}

void Processor::SetOpEvent(int32_t virtualPc)
{
    SetOpEvent(virtualPc, InvalidValue, InvalidValue, OpCode::unknown);
}

void Processor::SetOpEvent(int32_t virtualPc, PhysicalAddress physicalPc, int32_t insn, OpCode opCode)
{
    m_OpEvent.insn = insn;
    m_OpEvent.opCode = opCode;
    m_OpEvent.opId = m_OpCount;
    m_OpEvent.virtualPc = virtualPc;
    m_OpEvent.physicalPc = physicalPc;

    m_OpEventValid = true;

    m_OpCount++;
}
