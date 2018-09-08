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

#include "ProcessorException.h"
#include "ControlStatusRegister.h"
#include "Processor.h"

using namespace std;

namespace
{
    inline int32_t sext(int8_t value)
    {
        return static_cast<int32_t>(value);
    }

    inline int32_t sext(int16_t value)
    {
        return static_cast<int32_t>(value);
    }
}

void Processor::SetIntReg(int regId, int32_t regValue)
{
    m_IntReg.Write(regId, regValue);
}

void Processor::ProcessOneCycle()
{
    // clear event for Dump
    m_OpEventValid = false;

    m_Csr.ClearEvent();
    m_Memory.ClearEvent();

    // Fetch Op
    Op op { OpClass::RV32I, OpCode::unknown };
    int32_t pc = InvalidValue;
    int32_t insn = InvalidValue;
    PhysicalAddress physicalPc = InvalidValue;

    try
    {
        pc = m_Csr.GetProgramCounter();
        m_Memory.CheckException(MemoryAccessType::Instruction, pc, pc);
        insn = m_Memory.FetchInstruction(&physicalPc, pc);

        m_Decoder.Decode(&op, insn);

        if (op.opCode == OpCode::unknown)
        {
            throw IllegalInstructionException(pc, insn);
        }

        PreCheckException(op, pc, insn);

        m_Csr.SetProgramCounter(pc + 4);
        ProcessOp(op, pc);

        PostCheckException(op, pc);
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

void Processor::PreCheckException(const Op& op, int32_t pc, int32_t insn)
{
    int32_t memoryAddress;
    switch (op.opCode)
    {
    case OpCode::lb:
    case OpCode::lh:
    case OpCode::lw:
    case OpCode::lbu:
    case OpCode::lhu:
        memoryAddress = m_IntReg.Read(op.rs1) + op.imm;
        m_Memory.CheckException(MemoryAccessType::Load, pc, memoryAddress);
        break;
    case OpCode::sb:
    case OpCode::sh:
    case OpCode::sw:
        memoryAddress = m_IntReg.Read(op.rs1) + op.imm;
        m_Memory.CheckException(MemoryAccessType::Store, pc, memoryAddress);
        break;
    case OpCode::csrrw:
    case OpCode::csrrs:
    case OpCode::csrrc:
    case OpCode::csrrwi:
    case OpCode::csrrsi:
    case OpCode::csrrci:
        m_Csr.CheckException(op.csr, op.rs1 != 0, pc, insn);
        break;
    case OpCode::lr_w:
        memoryAddress = m_IntReg.Read(op.rs1);
        m_Memory.CheckException(MemoryAccessType::Load, pc, memoryAddress);
        break;
    case OpCode::sc_w:
        memoryAddress = m_IntReg.Read(op.rs1);
        m_Memory.CheckException(MemoryAccessType::Store, pc, memoryAddress);
        break;
    case OpCode::amoswap_w:
    case OpCode::amoadd_w:
    case OpCode::amoxor_w:
    case OpCode::amoand_w:
    case OpCode::amoor_w:
    case OpCode::amomin_w:
    case OpCode::amomax_w:
    case OpCode::amominu_w:
    case OpCode::amomaxu_w:
        memoryAddress = m_IntReg.Read(op.rs1);
        m_Memory.CheckException(MemoryAccessType::Load, pc, memoryAddress);
        m_Memory.CheckException(MemoryAccessType::Store, pc, memoryAddress);
    default:
        break;
    }
}

void Processor::PostCheckException(const Op& op, int32_t pc)
{
    const auto privilegeLevel = m_Csr.GetPrivilegeLevel();

    switch (op.opCode)
    {
    case OpCode::ecall:
        if (privilegeLevel == PrivilegeLevel::User)
        {
            throw EnvironmentCallFromUserException(pc);
        }
        if (privilegeLevel == PrivilegeLevel::Supervisor)
        {
            throw EnvironmentCallFromSupervisorException(pc);
        }
        if (privilegeLevel == PrivilegeLevel::Machine)
        {
            throw EnvironmentCallFromMachineException(pc);
        }
        else
        {
            throw NotImplementedException(__FILE__, __LINE__);
        }
        break;
    case OpCode::ebreak:
        throw BreakpointException(pc);
    default:
        break;
    }
}

void Processor::ProcessOp(const Op& op, int32_t pc)
{
    switch (op.opClass)
    {
    case OpClass::RV32I:
        ProcessRV32I(op, pc);
        break;
    case OpClass::RV32M:
        ProcessRV32M(op);
        break;
    case OpClass::RV32A:
        ProcessRV32A(op);
        break;
    default:
        throw NotImplementedException(__FILE__, __LINE__);
    }
}

void Processor::ProcessRV32I(const Op& op, int32_t pc)
{
    int32_t address;
    int32_t tmp;

    const int32_t rs1 = m_IntReg.Read(op.rs1);

    switch (op.opCode)
    {
        // RV32I
    case OpCode::lui:
        m_IntReg.Write(op.rd, op.imm);
        break;
    case OpCode::auipc:
        m_IntReg.Write(op.rd, pc + op.imm);
        break;
    case OpCode::jal:
        m_IntReg.Write(op.rd, pc + 4);
        m_Csr.SetProgramCounter(pc + op.imm);
        break;
    case OpCode::jalr:
        m_IntReg.Write(op.rd, pc + 4);
        m_Csr.SetProgramCounter(rs1 + op.imm);
        break;
    case OpCode::beq:
        if (m_IntReg.Read(op.rs1) == m_IntReg.Read(op.rs2))
        {
            m_Csr.SetProgramCounter(pc + op.imm);
        }
        break;
    case OpCode::bne:
        if (m_IntReg.Read(op.rs1) != m_IntReg.Read(op.rs2))
        {
            m_Csr.SetProgramCounter(pc + op.imm);
        }
        break;
    case OpCode::blt:
        if (m_IntReg.Read(op.rs1) < m_IntReg.Read(op.rs2))
        {
            m_Csr.SetProgramCounter(pc + op.imm);
        }
        break;
    case OpCode::bge:
        if (m_IntReg.Read(op.rs1) >= m_IntReg.Read(op.rs2))
        {
            m_Csr.SetProgramCounter(pc + op.imm);
        }
        break;
    case OpCode::bltu:
        if (static_cast<uint32_t>(m_IntReg.Read(op.rs1)) < static_cast<uint32_t>(m_IntReg.Read(op.rs2)))
        {
            m_Csr.SetProgramCounter(pc + op.imm);
        }
        break;
    case OpCode::bgeu:
        if (static_cast<uint32_t>(m_IntReg.Read(op.rs1)) >= static_cast<uint32_t>(m_IntReg.Read(op.rs2)))
        {
            m_Csr.SetProgramCounter(pc + op.imm);
        }
        break;
    case OpCode::lb:
        address = m_IntReg.Read(op.rs1) + op.imm;
        m_IntReg.Write(op.rd, m_Memory.LoadInt8(address));
        break;
    case OpCode::lh:
        address = m_IntReg.Read(op.rs1) + op.imm;
        m_IntReg.Write(op.rd, m_Memory.LoadInt16(address));
        break;
    case OpCode::lw:
        address = m_IntReg.Read(op.rs1) + op.imm;
        m_IntReg.Write(op.rd, m_Memory.LoadInt32(address));
        break;
    case OpCode::lbu:
        address = m_IntReg.Read(op.rs1) + op.imm;
        m_IntReg.Write(op.rd, m_Memory.LoadInt8(address) & 0x000000ff);
        break;
    case OpCode::lhu:
        address = m_IntReg.Read(op.rs1) + op.imm;
        m_IntReg.Write(op.rd, m_Memory.LoadInt16(address) & 0x0000ffff);
        break;
    case OpCode::sb:
        address = m_IntReg.Read(op.rs1) + op.imm;
        tmp = m_IntReg.Read(op.rs2);
        m_Memory.StoreInt8(address, static_cast<int8_t>(tmp));
        break;
    case OpCode::sh:
        address = m_IntReg.Read(op.rs1) + op.imm;
        tmp = m_IntReg.Read(op.rs2);
        m_Memory.StoreInt16(address, static_cast<int16_t>(tmp));
        break;
    case OpCode::sw:
        address = m_IntReg.Read(op.rs1) + op.imm;
        tmp = m_IntReg.Read(op.rs2);
        m_Memory.StoreInt32(address, static_cast<int32_t>(tmp));
        break;
    case OpCode::addi:
        m_IntReg.Write(op.rd, m_IntReg.Read(op.rs1) + op.imm);
        break;
    case OpCode::slti:
        m_IntReg.Write(op.rd, (m_IntReg.Read(op.rs1) < op.imm) ? 1 : 0);
        break;
    case OpCode::sltiu:
        m_IntReg.Write(op.rd, (static_cast<uint32_t>(m_IntReg.Read(op.rs1)) < static_cast<uint32_t>(op.imm)) ? 1 : 0);
        break;
    case OpCode::xori:
        m_IntReg.Write(op.rd, m_IntReg.Read(op.rs1) ^ op.imm);
        break;
    case OpCode::ori:
        m_IntReg.Write(op.rd, m_IntReg.Read(op.rs1) | op.imm);
        break;
    case OpCode::andi:
        m_IntReg.Write(op.rd, m_IntReg.Read(op.rs1) & op.imm);
        break;
    case OpCode::slli:
        m_IntReg.Write(op.rd, m_IntReg.Read(op.rs1) << op.rs2);
        break;
    case OpCode::srli:
        m_IntReg.Write(op.rd, static_cast<uint32_t>(m_IntReg.Read(op.rs1)) >> op.rs2);
        break;
    case OpCode::srai:
        m_IntReg.Write(op.rd, m_IntReg.Read(op.rs1) >> op.rs2);
        break;
    case OpCode::add:
        m_IntReg.Write(op.rd, m_IntReg.Read(op.rs1) + m_IntReg.Read(op.rs2));
        break;
    case OpCode::sub:
        m_IntReg.Write(op.rd, m_IntReg.Read(op.rs1) - m_IntReg.Read(op.rs2));
        break;
    case OpCode::sll:
        m_IntReg.Write(op.rd, m_IntReg.Read(op.rs1) << m_IntReg.Read(op.rs2));
        break;
    case OpCode::slt:
        m_IntReg.Write(op.rd, (m_IntReg.Read(op.rs1) < m_IntReg.Read(op.rs2)) ? 1 : 0);
        break;
    case OpCode::sltu:
        m_IntReg.Write(op.rd, (static_cast<uint32_t>(m_IntReg.Read(op.rs1)) < static_cast<uint32_t>(m_IntReg.Read(op.rs2))) ? 1 : 0);
        break;
    case OpCode::xor_:
        m_IntReg.Write(op.rd, m_IntReg.Read(op.rs1) ^ m_IntReg.Read(op.rs2));
        break;
    case OpCode::srl:
        m_IntReg.Write(op.rd, static_cast<uint32_t>(m_IntReg.Read(op.rs1)) >> m_IntReg.Read(op.rs2));
        break;
    case OpCode::sra:
        m_IntReg.Write(op.rd, m_IntReg.Read(op.rs1) >> m_IntReg.Read(op.rs2));
        break;
    case OpCode::or_:
        m_IntReg.Write(op.rd, m_IntReg.Read(op.rs1) | m_IntReg.Read(op.rs2));
        break;
    case OpCode::and_:
        m_IntReg.Write(op.rd, m_IntReg.Read(op.rs1) & m_IntReg.Read(op.rs2));
        break;
    case OpCode::fence:
        // Do nothing for memory fence instructions.
        break;
    case OpCode::fence_i:
        // Do nothing for memory fence instructions.
        break;
    case OpCode::ecall:
        break;
    case OpCode::ebreak:
        break;
    case OpCode::csrrw:
        tmp = m_Csr.Read(op.csr);
        m_Csr.Write(op.csr, m_IntReg.Read(op.rs1));
        m_IntReg.Write(op.rd, tmp);
        break;
    case OpCode::csrrs:
        tmp = m_Csr.Read(op.csr);
        m_Csr.Write(op.csr, tmp | m_IntReg.Read(op.rs1));
        m_IntReg.Write(op.rd, tmp);
        break;
    case OpCode::csrrc:
        tmp = m_Csr.Read(op.csr);
        m_Csr.Write(op.csr, tmp & ~m_IntReg.Read(op.rs1));
        m_IntReg.Write(op.rd, tmp);
        break;
    case OpCode::csrrwi:
        tmp = m_Csr.Read(op.csr);
        m_Csr.Write(op.csr, op.zimm);
        m_IntReg.Write(op.rd, tmp);
        break;
    case OpCode::csrrsi:
        tmp = m_Csr.Read(op.csr);
        m_Csr.Write(op.csr, tmp | op.zimm);
        m_IntReg.Write(op.rd, tmp);
        break;
    case OpCode::csrrci:
        tmp = m_Csr.Read(op.csr);
        m_Csr.Write(op.csr, tmp & ~op.zimm);
        m_IntReg.Write(op.rd, tmp);
        break;
    case OpCode::mret:
        m_Csr.ProcessTrapReturn(PrivilegeLevel::Machine);
        break;
    case OpCode::sret:
        m_Csr.ProcessTrapReturn(PrivilegeLevel::Supervisor);
        break;
    case OpCode::uret:
        m_Csr.ProcessTrapReturn(PrivilegeLevel::User);
        break;
    case OpCode::wfi:
        // Interrupt is not implemented.
        break;
    case OpCode::sfence_vma:
        // Do nothing for memory fence instructions.
        break;
    default:
        throw NotImplementedException(__FILE__, __LINE__);
    }
}

void Processor::ProcessRV32M(const Op& op)
{
    int32_t dst;

    int32_t src1 = m_IntReg.Read(op.rs1);
    int32_t src2 = m_IntReg.Read(op.rs2);

    uint32_t src1_u = static_cast<uint32_t>(src1);
    uint32_t src2_u = static_cast<uint32_t>(src2);

    switch (op.opCode)
    {
    case OpCode::mul:
        dst = src1 * src2;
        break;
    case OpCode::mulh:
        dst = static_cast<int32_t>((static_cast<int64_t>(src1) * static_cast<int64_t>(src2)) >> 32);
        break;
    case OpCode::mulhsu:
        dst = static_cast<int32_t>((static_cast<int64_t>(src1) * static_cast<uint64_t>(src2_u)) >> 32);
        break;
    case OpCode::mulhu:
        dst = static_cast<int32_t>((static_cast<uint64_t>(src1_u) * static_cast<uint64_t>(src2_u)) >> 32);
        break;
    case OpCode::div:
        if (src1_u == 0x80000000 && src2 == -1)
        {
            dst = 0x80000000;
        }
        else if (src2 == 0)
        {
            dst = -1;
        }
        else
        {
            dst = src1 / src2;
        }
        break;
    case OpCode::divu:
        if (src2 == 0)
        {
            dst = -1;
        }
        else
        {
            dst = static_cast<int32_t>(src1_u / src2_u);
        }
        break;
    case OpCode::rem:
        if (src1_u == 0x80000000 && src2 == -1)
        {
            dst = 0;
        }
        else if (src2 == 0)
        {
            dst = src1;
        }
        else
        {
            dst = src1 % src2;
        }
        break;
    case OpCode::remu:
        if (src2 == 0)
        {
            dst = src1;
        }
        else
        {
            dst = static_cast<int32_t>(src1_u % src2_u);
        }
        break;
    default:
        throw NotImplementedException(__FILE__, __LINE__);
    }

    m_IntReg.Write(op.rd, dst);
}

void Processor::ProcessRV32A(const Op& op)
{
    int32_t mem;

    int32_t src1 = m_IntReg.Read(op.rs1);
    int32_t src2 = m_IntReg.Read(op.rs2);

    switch (op.opCode)
    {
    case OpCode::lr_w:
        m_ReserveAddress = src1;
        mem = m_Memory.LoadInt32(src1);
        m_IntReg.Write(op.rd, mem);
        break;
    case OpCode::sc_w:
        if (m_ReserveAddress == src1)
        {
            m_Memory.StoreInt32(src1, src2);
            m_IntReg.Write(op.rd, 0);
        }
        else
        {
            m_IntReg.Write(op.rd, 1);
        }
        break;
    case OpCode::amoswap_w:
        mem = m_Memory.LoadInt32(src1);
        m_Memory.StoreInt32(src1, src2);
        m_IntReg.Write(op.rd, mem);
        break;
    case OpCode::amoadd_w:
        mem = m_Memory.LoadInt32(src1);
        m_Memory.StoreInt32(src1, mem + src2);
        m_IntReg.Write(op.rd, mem);
        break;
    case OpCode::amoxor_w:
        mem = m_Memory.LoadInt32(src1);
        m_Memory.StoreInt32(src1, mem ^ src2);
        m_IntReg.Write(op.rd, mem);
        break;
    case OpCode::amoand_w:
        mem = m_Memory.LoadInt32(src1);
        m_Memory.StoreInt32(src1, mem & src2);
        m_IntReg.Write(op.rd, mem);
        break;
    case OpCode::amoor_w:
        mem = m_Memory.LoadInt32(src1);
        m_Memory.StoreInt32(src1, mem | src2);
        m_IntReg.Write(op.rd, mem);
        break;
    case OpCode::amomax_w:
        mem = m_Memory.LoadInt32(src1);
        m_Memory.StoreInt32(src1, std::max(mem, src2));
        m_IntReg.Write(op.rd, mem);
        break;
    case OpCode::amomin_w:
        mem = m_Memory.LoadInt32(src1);
        m_Memory.StoreInt32(src1, std::min(mem, src2));
        m_IntReg.Write(op.rd, mem);
        break;
    case OpCode::amomaxu_w:
        mem = m_Memory.LoadInt32(src1);
        m_Memory.StoreInt32(src1, std::max(static_cast<uint32_t>(mem), static_cast<uint32_t>(src2)));
        m_IntReg.Write(op.rd, mem);
        break;
    case OpCode::amominu_w:
        mem = m_Memory.LoadInt32(src1);
        m_Memory.StoreInt32(src1, std::min(static_cast<uint32_t>(mem), static_cast<uint32_t>(src2)));
        m_IntReg.Write(op.rd, mem);
        break;
    default:
        throw NotImplementedException(__FILE__, __LINE__);
    }
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
    m_IntReg.Copy(pOut, size);
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
    m_Memory.CopyEvent(pOut);
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
    return m_Memory.IsEventExist();
}

bool Processor::IsTrapEventExist() const
{
    return m_Csr.IsTrapEventExist();
}
