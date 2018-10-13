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

#include <rafi/Macro.h>

#include "Executor.h"

using namespace std;
using namespace rvtrace;

namespace rafi { namespace cpu {

std::optional<Trap> Executor::PreCheckTrap(const Op& op, int32_t pc, int32_t insn) const
{
    switch (op.opCode)
    {
    case OpCode::lb:
    case OpCode::lh:
    case OpCode::lw:
    case OpCode::lbu:
    case OpCode::lhu:
        return PreCheckTrapForLoad(pc, m_pIntRegFile->Read(op.rs1) + op.imm);
    case OpCode::sb:
    case OpCode::sh:
    case OpCode::sw:
        return PreCheckTrapForStore(pc, m_pIntRegFile->Read(op.rs1) + op.imm);
    case OpCode::csrrw:
    case OpCode::csrrs:
    case OpCode::csrrc:
    case OpCode::csrrwi:
    case OpCode::csrrsi:
    case OpCode::csrrci:
        return PreCheckTrapForCsr(op, pc, insn);
    case OpCode::lr_w:
        return PreCheckTrapForLoad(pc, m_pIntRegFile->Read(op.rs1));
        break;
    case OpCode::sc_w:
        return PreCheckTrapForStore(pc, m_pIntRegFile->Read(op.rs1));
    case OpCode::amoswap_w:
    case OpCode::amoadd_w:
    case OpCode::amoxor_w:
    case OpCode::amoand_w:
    case OpCode::amoor_w:
    case OpCode::amomin_w:
    case OpCode::amomax_w:
    case OpCode::amominu_w:
    case OpCode::amomaxu_w:
        return PreCheckTrapForAtomic(pc, m_pIntRegFile->Read(op.rs1));
    default:
        return std::nullopt;
    }
}

std::optional<Trap> Executor::PostCheckTrap(const Op& op, int32_t pc) const
{
    switch (op.opCode)
    {
    case OpCode::ecall:
        return PostCheckTrapForEcall(pc);
    case OpCode::ebreak:
        return MakeBreakpointException(pc);
    default:
        return std::nullopt;
    }
}

void Executor::ProcessOp(const Op& op, int32_t pc)
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

std::optional<Trap> Executor::PreCheckTrapForLoad(int32_t pc, int32_t address) const
{
    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapForStore(int32_t pc, int32_t address) const
{
    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapForCsr(const Op& op, int32_t pc, int32_t insn) const
{
    return m_pCsr->CheckTrap(op.csr, op.rs1 != 0, pc, insn);
}

std::optional<Trap> Executor::PreCheckTrapForAtomic(int32_t pc, int32_t address) const
{
    const auto trap = m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);

    if (trap)
    {
        return trap;
    }

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PostCheckTrapForEcall(int32_t pc) const
{
    const auto privilegeLevel = m_pCsr->GetPrivilegeLevel();

    switch (privilegeLevel)
    {
    case PrivilegeLevel::Machine:
        return MakeEnvironmentCallFromMachineException(pc);
    case PrivilegeLevel::Supervisor:
        return MakeEnvironmentCallFromSupervisorException(pc);
    case PrivilegeLevel::User:
        return MakeEnvironmentCallFromUserException(pc);
    default:
        ABORT();
    }
}

void Executor::ProcessRV32I(const Op& op, int32_t pc)
{
    int32_t address;
    int32_t tmp;

    const int32_t rs1 = m_pIntRegFile->Read(op.rs1);

    switch (op.opCode)
    {
        // RV32I
    case OpCode::lui:
        m_pIntRegFile->Write(op.rd, op.imm);
        break;
    case OpCode::auipc:
        m_pIntRegFile->Write(op.rd, pc + op.imm);
        break;
    case OpCode::jal:
        m_pIntRegFile->Write(op.rd, pc + 4);
        m_pCsr->SetProgramCounter(pc + op.imm);
        break;
    case OpCode::jalr:
        m_pIntRegFile->Write(op.rd, pc + 4);
        m_pCsr->SetProgramCounter(rs1 + op.imm);
        break;
    case OpCode::beq:
        if (m_pIntRegFile->Read(op.rs1) == m_pIntRegFile->Read(op.rs2))
        {
            m_pCsr->SetProgramCounter(pc + op.imm);
        }
        break;
    case OpCode::bne:
        if (m_pIntRegFile->Read(op.rs1) != m_pIntRegFile->Read(op.rs2))
        {
            m_pCsr->SetProgramCounter(pc + op.imm);
        }
        break;
    case OpCode::blt:
        if (m_pIntRegFile->Read(op.rs1) < m_pIntRegFile->Read(op.rs2))
        {
            m_pCsr->SetProgramCounter(pc + op.imm);
        }
        break;
    case OpCode::bge:
        if (m_pIntRegFile->Read(op.rs1) >= m_pIntRegFile->Read(op.rs2))
        {
            m_pCsr->SetProgramCounter(pc + op.imm);
        }
        break;
    case OpCode::bltu:
        if (static_cast<uint32_t>(m_pIntRegFile->Read(op.rs1)) < static_cast<uint32_t>(m_pIntRegFile->Read(op.rs2)))
        {
            m_pCsr->SetProgramCounter(pc + op.imm);
        }
        break;
    case OpCode::bgeu:
        if (static_cast<uint32_t>(m_pIntRegFile->Read(op.rs1)) >= static_cast<uint32_t>(m_pIntRegFile->Read(op.rs2)))
        {
            m_pCsr->SetProgramCounter(pc + op.imm);
        }
        break;
    case OpCode::lb:
        address = m_pIntRegFile->Read(op.rs1) + op.imm;
        m_pIntRegFile->Write(op.rd, m_pMemAccessUnit->LoadInt8(address));
        break;
    case OpCode::lh:
        address = m_pIntRegFile->Read(op.rs1) + op.imm;
        m_pIntRegFile->Write(op.rd, m_pMemAccessUnit->LoadInt16(address));
        break;
    case OpCode::lw:
        address = m_pIntRegFile->Read(op.rs1) + op.imm;
        m_pIntRegFile->Write(op.rd, m_pMemAccessUnit->LoadInt32(address));
        break;
    case OpCode::lbu:
        address = m_pIntRegFile->Read(op.rs1) + op.imm;
        m_pIntRegFile->Write(op.rd, m_pMemAccessUnit->LoadInt8(address) & 0x000000ff);
        break;
    case OpCode::lhu:
        address = m_pIntRegFile->Read(op.rs1) + op.imm;
        m_pIntRegFile->Write(op.rd, m_pMemAccessUnit->LoadInt16(address) & 0x0000ffff);
        break;
    case OpCode::sb:
        address = m_pIntRegFile->Read(op.rs1) + op.imm;
        tmp = m_pIntRegFile->Read(op.rs2);
        m_pMemAccessUnit->StoreInt8(address, static_cast<int8_t>(tmp));
        break;
    case OpCode::sh:
        address = m_pIntRegFile->Read(op.rs1) + op.imm;
        tmp = m_pIntRegFile->Read(op.rs2);
        m_pMemAccessUnit->StoreInt16(address, static_cast<int16_t>(tmp));
        break;
    case OpCode::sw:
        address = m_pIntRegFile->Read(op.rs1) + op.imm;
        tmp = m_pIntRegFile->Read(op.rs2);
        m_pMemAccessUnit->StoreInt32(address, static_cast<int32_t>(tmp));
        break;
    case OpCode::addi:
        m_pIntRegFile->Write(op.rd, m_pIntRegFile->Read(op.rs1) + op.imm);
        break;
    case OpCode::slti:
        m_pIntRegFile->Write(op.rd, (m_pIntRegFile->Read(op.rs1) < op.imm) ? 1 : 0);
        break;
    case OpCode::sltiu:
        m_pIntRegFile->Write(op.rd, (static_cast<uint32_t>(m_pIntRegFile->Read(op.rs1)) < static_cast<uint32_t>(op.imm)) ? 1 : 0);
        break;
    case OpCode::xori:
        m_pIntRegFile->Write(op.rd, m_pIntRegFile->Read(op.rs1) ^ op.imm);
        break;
    case OpCode::ori:
        m_pIntRegFile->Write(op.rd, m_pIntRegFile->Read(op.rs1) | op.imm);
        break;
    case OpCode::andi:
        m_pIntRegFile->Write(op.rd, m_pIntRegFile->Read(op.rs1) & op.imm);
        break;
    case OpCode::slli:
        m_pIntRegFile->Write(op.rd, m_pIntRegFile->Read(op.rs1) << op.rs2);
        break;
    case OpCode::srli:
        m_pIntRegFile->Write(op.rd, static_cast<uint32_t>(m_pIntRegFile->Read(op.rs1)) >> op.rs2);
        break;
    case OpCode::srai:
        m_pIntRegFile->Write(op.rd, m_pIntRegFile->Read(op.rs1) >> op.rs2);
        break;
    case OpCode::add:
        m_pIntRegFile->Write(op.rd, m_pIntRegFile->Read(op.rs1) + m_pIntRegFile->Read(op.rs2));
        break;
    case OpCode::sub:
        m_pIntRegFile->Write(op.rd, m_pIntRegFile->Read(op.rs1) - m_pIntRegFile->Read(op.rs2));
        break;
    case OpCode::sll:
        m_pIntRegFile->Write(op.rd, m_pIntRegFile->Read(op.rs1) << m_pIntRegFile->Read(op.rs2));
        break;
    case OpCode::slt:
        m_pIntRegFile->Write(op.rd, (m_pIntRegFile->Read(op.rs1) < m_pIntRegFile->Read(op.rs2)) ? 1 : 0);
        break;
    case OpCode::sltu:
        m_pIntRegFile->Write(op.rd, (static_cast<uint32_t>(m_pIntRegFile->Read(op.rs1)) < static_cast<uint32_t>(m_pIntRegFile->Read(op.rs2))) ? 1 : 0);
        break;
    case OpCode::xor_:
        m_pIntRegFile->Write(op.rd, m_pIntRegFile->Read(op.rs1) ^ m_pIntRegFile->Read(op.rs2));
        break;
    case OpCode::srl:
        m_pIntRegFile->Write(op.rd, static_cast<uint32_t>(m_pIntRegFile->Read(op.rs1)) >> m_pIntRegFile->Read(op.rs2));
        break;
    case OpCode::sra:
        m_pIntRegFile->Write(op.rd, m_pIntRegFile->Read(op.rs1) >> m_pIntRegFile->Read(op.rs2));
        break;
    case OpCode::or_:
        m_pIntRegFile->Write(op.rd, m_pIntRegFile->Read(op.rs1) | m_pIntRegFile->Read(op.rs2));
        break;
    case OpCode::and_:
        m_pIntRegFile->Write(op.rd, m_pIntRegFile->Read(op.rs1) & m_pIntRegFile->Read(op.rs2));
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
        tmp = m_pCsrAccessor->Read(op.csr);
        m_pCsrAccessor->Write(op.csr, m_pIntRegFile->Read(op.rs1));
        m_pIntRegFile->Write(op.rd, tmp);
        break;
    case OpCode::csrrs:
        tmp = m_pCsrAccessor->Read(op.csr);
        m_pCsrAccessor->Write(op.csr, tmp | m_pIntRegFile->Read(op.rs1));
        m_pIntRegFile->Write(op.rd, tmp);
        break;
    case OpCode::csrrc:
        tmp = m_pCsrAccessor->Read(op.csr);
        m_pCsrAccessor->Write(op.csr, tmp & ~m_pIntRegFile->Read(op.rs1));
        m_pIntRegFile->Write(op.rd, tmp);
        break;
    case OpCode::csrrwi:
        tmp = m_pCsrAccessor->Read(op.csr);
        m_pCsrAccessor->Write(op.csr, op.zimm);
        m_pIntRegFile->Write(op.rd, tmp);
        break;
    case OpCode::csrrsi:
        tmp = m_pCsrAccessor->Read(op.csr);
        m_pCsrAccessor->Write(op.csr, tmp | op.zimm);
        m_pIntRegFile->Write(op.rd, tmp);
        break;
    case OpCode::csrrci:
        tmp = m_pCsrAccessor->Read(op.csr);
        m_pCsrAccessor->Write(op.csr, tmp & ~op.zimm);
        m_pIntRegFile->Write(op.rd, tmp);
        break;
    case OpCode::mret:
        m_pTrapProcessor->ProcessTrapReturn(PrivilegeLevel::Machine);
        break;
    case OpCode::sret:
        m_pTrapProcessor->ProcessTrapReturn(PrivilegeLevel::Supervisor);
        break;
    case OpCode::uret:
        m_pTrapProcessor->ProcessTrapReturn(PrivilegeLevel::User);
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

void Executor::ProcessRV32M(const Op& op)
{
    int32_t dst;

    int32_t src1 = m_pIntRegFile->Read(op.rs1);
    int32_t src2 = m_pIntRegFile->Read(op.rs2);

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

    m_pIntRegFile->Write(op.rd, dst);
}

void Executor::ProcessRV32A(const Op& op)
{
    int32_t mem;

    int32_t src1 = m_pIntRegFile->Read(op.rs1);
    int32_t src2 = m_pIntRegFile->Read(op.rs2);

    switch (op.opCode)
    {
    case OpCode::lr_w:
        m_ReserveAddress = src1;
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pIntRegFile->Write(op.rd, mem);
        break;
    case OpCode::sc_w:
        if (m_ReserveAddress == src1)
        {
            m_pMemAccessUnit->StoreInt32(src1, src2);
            m_pIntRegFile->Write(op.rd, 0);
        }
        else
        {
            m_pIntRegFile->Write(op.rd, 1);
        }
        break;
    case OpCode::amoswap_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, src2);
        m_pIntRegFile->Write(op.rd, mem);
        break;
    case OpCode::amoadd_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, mem + src2);
        m_pIntRegFile->Write(op.rd, mem);
        break;
    case OpCode::amoxor_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, mem ^ src2);
        m_pIntRegFile->Write(op.rd, mem);
        break;
    case OpCode::amoand_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, mem & src2);
        m_pIntRegFile->Write(op.rd, mem);
        break;
    case OpCode::amoor_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, mem | src2);
        m_pIntRegFile->Write(op.rd, mem);
        break;
    case OpCode::amomax_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, std::max(mem, src2));
        m_pIntRegFile->Write(op.rd, mem);
        break;
    case OpCode::amomin_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, std::min(mem, src2));
        m_pIntRegFile->Write(op.rd, mem);
        break;
    case OpCode::amomaxu_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, std::max(static_cast<uint32_t>(mem), static_cast<uint32_t>(src2)));
        m_pIntRegFile->Write(op.rd, mem);
        break;
    case OpCode::amominu_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, std::min(static_cast<uint32_t>(mem), static_cast<uint32_t>(src2)));
        m_pIntRegFile->Write(op.rd, mem);
        break;
    default:
        throw NotImplementedException(__FILE__, __LINE__);
    }
}

}}
