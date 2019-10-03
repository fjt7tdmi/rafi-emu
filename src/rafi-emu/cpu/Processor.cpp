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

#include <rafi/common.h>

#include "Processor.h"

namespace rafi { namespace emu { namespace cpu {

Processor::Processor(XLEN xlen, bus::Bus* pBus, vaddr_t initialPc)
    : m_Csr(xlen, initialPc)
    , m_InterruptController(&m_Csr)
    , m_TrapProcessor(xlen, &m_Csr)
    , m_Decoder(xlen)
    , m_MemAccessUnit(xlen)
    , m_Executor(&m_AtomicManager, &m_Csr, &m_TrapProcessor, &m_IntRegFile, &m_FpRegFile, &m_MemAccessUnit)
{
    m_MemAccessUnit.Initialize(pBus, &m_Csr);
}

void Processor::RegisterExternalInterruptSource(IInterruptSource* pInterruptSource)
{
    m_InterruptController.RegisterExternalInterruptSource(pInterruptSource);
}

void Processor::RegisterTimerInterruptSource(IInterruptSource* pInterruptSource)
{
    m_InterruptController.RegisterTimerInterruptSource(pInterruptSource);
}

void Processor::SetIntReg(int regId, uint32_t regValue)
{
    m_IntRegFile.WriteUInt32(regId, regValue);
}

xip_t Processor::ReadInterruptPending() const
{
    return m_Csr.ReadInterruptPending();
}

void Processor::WriteInterruptPending(const xip_t& value)
{
    m_Csr.WriteInterruptPending(value);
}

uint64_t Processor::ReadTime() const
{
    return m_Csr.ReadTime();
}

void Processor::WriteTime(uint64_t value)
{
    m_Csr.WriteTime(value);
}

void Processor::ProcessCycle(Profiler* pProfiler)
{
    ClearOpEvent();
    m_TrapProcessor.ClearEvent();
    m_MemAccessUnit.ClearEvent();

    m_Csr.ProcessCycle();

    const auto privilegeLevel = m_Csr.GetPrivilegeLevel();
    const auto pc = m_Csr.GetProgramCounter();

    // Check interrupt
    m_InterruptController.Update();

    if (m_InterruptController.IsRequested())
    {
        const auto interruptType = m_InterruptController.GetInterruptType();

        m_TrapProcessor.ProcessInterrupt(interruptType, pc);

        SetOpEvent(pc, privilegeLevel);
        return;
    }

    // Fetch
    pProfiler->Switch(Profiler::Phase_Fetch);

    uint32_t insn;

    const auto fetchTrap = Fetch(&insn, pc);
    if (fetchTrap)
    {
        m_TrapProcessor.ProcessException(fetchTrap.value());

        SetOpEvent(pc, privilegeLevel);
        return;
    }

    // Decode
    pProfiler->Switch(Profiler::Phase_Decode);

    const auto op = m_Decoder.Decode(insn);
    if (op.opCode == OpCode::unknown)
    {
        const auto decodeTrap = MakeIllegalInstructionException(pc, insn);

        m_TrapProcessor.ProcessException(decodeTrap);
        return;
    }

    // Execute
    pProfiler->Switch(Profiler::Phase_Execute);

    const auto preExecuteTrap = m_Executor.PreCheckTrap(op, pc, insn);
    if (preExecuteTrap)
    {
        m_TrapProcessor.ProcessException(preExecuteTrap.value());
        return;
    }

    if (m_Decoder.IsCompressedInstruction(insn))
    {
        m_Csr.SetProgramCounter(pc + 2);
    }
    else
    {
        m_Csr.SetProgramCounter(pc + 4);
    }

    m_Executor.ProcessOp(op, pc);

    auto postExecuteTrap = m_Executor.PostCheckTrap(op, pc);
    if (postExecuteTrap)
    {
        m_TrapProcessor.ProcessException(postExecuteTrap.value());
        return;
    }
}

vaddr_t Processor::GetPc() const
{
    return m_Csr.GetProgramCounter();
}

int Processor::GetCsrCount() const
{
    return m_Csr.GetRegCount();
}

size_t Processor::GetMemoryAccessEventCount() const
{
    return m_MemAccessUnit.GetEventCount();
}

void Processor::CopyIntReg(trace::NodeIntReg32* pOut) const
{
    m_IntRegFile.Copy(pOut);
}

void Processor::CopyIntReg(trace::NodeIntReg64* pOut) const
{
    m_IntRegFile.Copy(pOut);
}

void Processor::CopyCsr(trace::Csr32Node* pOutNodes, int nodeCount) const
{
    m_Csr.Copy(pOutNodes, nodeCount);
}

void Processor::CopyCsr(trace::Csr64Node* pOutNodes, int nodeCount) const
{
    m_Csr.Copy(pOutNodes, nodeCount);
}

void Processor::CopyFpReg(void* pOut, size_t size) const
{
    m_FpRegFile.Copy(pOut, size);
}

void Processor::CopyOpEvent(OpEvent* pOut) const
{
    std::memcpy(pOut, &m_OpEvent, sizeof(*pOut));
}

void Processor::CopyMemoryAccessEvent(MemoryAccessEvent* pOut, int index) const
{
    m_MemAccessUnit.CopyEvent(pOut, index);
}

void Processor::CopyTrapEvent(TrapEvent* pOut) const
{
    m_TrapProcessor.CopyTrapEvent(pOut);
}

bool Processor::IsOpEventExist() const
{
    return m_OpEventValid;
}

bool Processor::IsTrapEventExist() const
{
    return m_TrapProcessor.IsTrapEventExist();
}

std::optional<Trap> Processor::Fetch(uint32_t* pOutInsn, vaddr_t pc)
{
    if (pc % 0x1000 == 0xffe)
    {
        // To support 4-byte instruction across a page boundary, split memory access.
        const vaddr_t vaddrLow = pc;
        const vaddr_t vaddrHigh = pc + 2;

        paddr_t paddrLow;
        paddr_t paddrHigh;

        RAFI_RETURN_IF_TRAP(m_MemAccessUnit.Translate(&paddrLow, MemoryAccessType::Instruction, vaddrLow, pc));
        const auto insnLow = m_MemAccessUnit.FetchUInt16(vaddrLow, paddrLow);

        if (m_Decoder.IsCompressedInstruction(insnLow))
        {
            *pOutInsn = insnLow;
            return std::nullopt;
        }

        RAFI_RETURN_IF_TRAP(m_MemAccessUnit.Translate(&paddrHigh, MemoryAccessType::Instruction, vaddrHigh, pc));
        const auto insnHigh = m_MemAccessUnit.FetchUInt16(vaddrHigh, paddrHigh);

        *pOutInsn = (insnHigh << 16) | insnLow;
        return std::nullopt;
    }
    else
    {
        paddr_t paddr;
        RAFI_RETURN_IF_TRAP(m_MemAccessUnit.Translate(&paddr, MemoryAccessType::Instruction, pc, pc));

        *pOutInsn = m_MemAccessUnit.FetchUInt32(pc, paddr);
        return std::nullopt;
    }
}

void Processor::PrintStatus() const
{
    printf("    OpCount: %d (0x%x)\n", m_OpCount, m_OpCount);
    printf("    PC:      0x%016llx\n", static_cast<unsigned long long>(m_Csr.GetProgramCounter()));
}

void Processor::ClearOpEvent()
{
    m_OpEventValid = false;
}

void Processor::SetOpEvent(vaddr_t virtualPc, PrivilegeLevel privilegeLevel)
{
    SetOpEvent(virtualPc, InvalidValue, InvalidValue, privilegeLevel);
}

void Processor::SetOpEvent(vaddr_t virtualPc, paddr_t physicalPc, uint32_t insn, PrivilegeLevel privilegeLevel)
{
    m_OpEvent.opId = m_OpCount;
    m_OpEvent.insn = insn;
    m_OpEvent.privilegeLevel = privilegeLevel;
    m_OpEvent.virtualPc = virtualPc;
    m_OpEvent.physicalPc = physicalPc;

    m_OpEventValid = true;

    m_OpCount++;
}

}}}
