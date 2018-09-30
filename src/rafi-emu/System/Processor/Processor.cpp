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
    // clear event for Dump
    m_OpEventValid = false;

    m_Csr.ClearEvent();
    m_MemAccessUnit.ClearEvent();

    // TODO: Check interrupt

    // Fetch Op
    Op op { OpClass::RV32I, OpCode::unknown };
    int32_t pc = InvalidValue;
    int32_t insn = InvalidValue;
    PhysicalAddress physicalPc = InvalidValue;

    try
    {
        pc = m_Csr.GetProgramCounter();
        
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
        m_Csr.ProcessException(e);
    }

    // set event for Dump
    m_OpEvent.insn = insn;
    m_OpEvent.opCode = op.opCode;
    m_OpEvent.opId = m_OpCount;
    m_OpEvent.virtualPc = pc;
    m_OpEvent.physicalPc = physicalPc;

    m_OpEventValid = true;

    m_OpCount++;
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
    m_Csr.CopyReadEvent(pOut);
}

void Processor::CopyCsrWriteEvent(CsrWriteEvent* pOut) const
{
    m_Csr.CopyWriteEvent(pOut);
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
    m_Csr.CopyTrapEvent(pOut);
}

bool Processor::IsCsrReadEventExist() const
{
    return m_Csr.IsReadEventExist();
}

bool Processor::IsCsrWriteEventExist() const
{
    return m_Csr.IsWriteEventExist();
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
    return m_Csr.IsTrapEventExist();
}
