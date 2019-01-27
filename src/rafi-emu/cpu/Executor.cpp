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
#include <cfenv>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>

#include <rvtrace/common.h>
#include <rafi/Common.h>

#include "Executor.h"
#include "FpUtil.h"

#pragma fenv_access (on)

using namespace std;
using namespace rvtrace;

namespace rafi { namespace emu { namespace cpu {

std::optional<Trap> Executor::PreCheckTrap(const Op& op, int32_t pc, int32_t insn) const
{
    switch (op.opCode)
    {
    case OpCode::lb:
    case OpCode::lh:
    case OpCode::lw:
    case OpCode::lbu:
    case OpCode::lhu:
        return PreCheckTrapForLoad(op, pc);
    case OpCode::sb:
    case OpCode::sh:
    case OpCode::sw:
        return PreCheckTrapForStore(op, pc);
    case OpCode::csrrw:
    case OpCode::csrrs:
    case OpCode::csrrc:
        return PreCheckTrapForCsr(op, pc, insn);
    case OpCode::csrrwi:
    case OpCode::csrrsi:
    case OpCode::csrrci:
        return PreCheckTrapForCsrImm(op, pc, insn);
    case OpCode::lr_w:
        return PreCheckTrapForLoadReserved(op, pc);
        break;
    case OpCode::sc_w:
        return PreCheckTrapForStoreConditional(op, pc);
    case OpCode::amoswap_w:
    case OpCode::amoadd_w:
    case OpCode::amoxor_w:
    case OpCode::amoand_w:
    case OpCode::amoor_w:
    case OpCode::amomin_w:
    case OpCode::amomax_w:
    case OpCode::amominu_w:
    case OpCode::amomaxu_w:
        return PreCheckTrapForAtomic(op, pc);
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
    case OpClass::RV32F:
        ProcessRV32F(op);
        break;
    case OpClass::RV32D:
        ProcessRV32D(op);
        break;
    default:
        Error(op);
    }
}

std::optional<Trap> Executor::PreCheckTrapForLoad(const Op& op, int32_t pc) const
{
    const auto& operand = std::get<OperandI>(op.operand);
    const auto address = m_pIntRegFile->ReadInt32(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapForLoadReserved(const Op& op, int32_t pc) const
{
    const auto& operand = std::get<OperandR>(op.operand);
    const auto address = m_pIntRegFile->ReadInt32(operand.rs1);

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapForStore(const Op& op, int32_t pc) const
{
    const auto& operand = std::get<OperandS>(op.operand);
    const auto address = m_pIntRegFile->ReadInt32(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapForStoreConditional(const Op& op, int32_t pc) const
{
    const auto& operand = std::get<OperandR>(op.operand);
    const auto address = m_pIntRegFile->ReadInt32(operand.rs1);

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapForCsr(const Op& op, int32_t pc, int32_t insn) const
{
    const auto& operand = std::get<OperandCsr>(op.operand);
    return m_pCsr->CheckTrap(static_cast<int>(operand.csr), operand.rd != 0, pc, insn);
}

std::optional<Trap> Executor::PreCheckTrapForCsrImm(const Op& op, int32_t pc, int32_t insn) const
{
    const auto& operand = std::get<OperandCsrImm>(op.operand);
    return m_pCsr->CheckTrap(static_cast<int>(operand.csr), operand.rd != 0, pc, insn);
}

std::optional<Trap> Executor::PreCheckTrapForAtomic(const Op& op, int32_t pc) const
{
    const auto& operand = std::get<OperandR>(op.operand);
    const int32_t address = m_pIntRegFile->ReadInt32(operand.rs1);

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
    switch (op.opCode)
    {
    case OpCode::lui:
        ProcessLui(op);
        return;
    case OpCode::auipc:
        ProcessAuipc(op, pc);
        return;
    case OpCode::jal:
        ProcessJal(op, pc);
        return;
    case OpCode::jalr:
        ProcessJalr(op, pc);
        return;
    case OpCode::beq:
    case OpCode::bne:
    case OpCode::blt:
    case OpCode::bge:
    case OpCode::bltu:
    case OpCode::bgeu:
        ProcessBranch(op, pc);
        return;
    case OpCode::lb:
    case OpCode::lh:
    case OpCode::lw:
    case OpCode::lbu:
    case OpCode::lhu:
        ProcessLoad(op);
        return;
    case OpCode::sb:
    case OpCode::sh:
    case OpCode::sw:
        ProcessStore(op);
        return;
    case OpCode::addi:
    case OpCode::slti:
    case OpCode::sltiu:
    case OpCode::xori:
    case OpCode::ori:
    case OpCode::andi:
        ProcessAluImm(op);
        return;
    case OpCode::sll:
    case OpCode::srl:
    case OpCode::sra:
        ProcessShift(op);
        return;
    case OpCode::slli:
    case OpCode::srli:
    case OpCode::srai:
        ProcessShiftImm(op);
        return;
    case OpCode::add:
    case OpCode::sub:
    case OpCode::slt:
    case OpCode::sltu:
    case OpCode::xor_:
    case OpCode::or_:
    case OpCode::and_:
        ProcessAlu(op);
        return;
    case OpCode::ecall:
    case OpCode::ebreak:
    case OpCode::mret:
    case OpCode::sret:
    case OpCode::uret:
    case OpCode::wfi:
        ProcessPriv(op);
        return;
    case OpCode::fence:
    case OpCode::fence_i:
    case OpCode::sfence_vma:
        // Do nothing for memory fence instructions.
        return;
    case OpCode::csrrw:
    case OpCode::csrrs:
    case OpCode::csrrc:
        ProcessCsr(op);
        return;
    case OpCode::csrrwi:
    case OpCode::csrrsi:
    case OpCode::csrrci:
        ProcessCsrImm(op);
        return;
    default:
        Error(op);
    }
}

void Executor::ProcessRV32M(const Op& op)
{
    const int rs1 = std::get<OperandR>(op.operand).rs1;
    const int rs2 = std::get<OperandR>(op.operand).rs2;
    const int rd = std::get<OperandR>(op.operand).rd;

    const int32_t src1 = m_pIntRegFile->ReadInt32(rs1);
    const int32_t src2 = m_pIntRegFile->ReadInt32(rs2);

    const uint32_t src1_u = static_cast<uint32_t>(src1);
    const uint32_t src2_u = static_cast<uint32_t>(src2);

    int32_t dst;

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
        Error(op);
    }

    m_pIntRegFile->WriteInt32(std::get<OperandR>(op.operand).rd, dst);
}

void Executor::ProcessRV32A(const Op& op)
{
    const int rs1 = std::get<OperandR>(op.operand).rs1;
    const int rs2 = std::get<OperandR>(op.operand).rs2;
    const int rd = std::get<OperandR>(op.operand).rd;

    const int32_t src1 = m_pIntRegFile->ReadInt32(rs1);
    const int32_t src2 = m_pIntRegFile->ReadInt32(rs2);

    int32_t mem;

    switch (op.opCode)
    {
    case OpCode::lr_w:
        m_ReserveAddress = src1;
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pIntRegFile->WriteInt32(rd, mem);
        break;
    case OpCode::sc_w:
        if (m_ReserveAddress == src1)
        {
            m_pMemAccessUnit->StoreInt32(src1, src2);
            m_pIntRegFile->WriteInt32(rd, 0);
        }
        else
        {
            m_pIntRegFile->WriteInt32(rd, 1);
        }
        break;
    case OpCode::amoswap_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, src2);
        m_pIntRegFile->WriteInt32(rd, mem);
        break;
    case OpCode::amoadd_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, mem + src2);
        m_pIntRegFile->WriteInt32(rd, mem);
        break;
    case OpCode::amoxor_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, mem ^ src2);
        m_pIntRegFile->WriteInt32(rd, mem);
        break;
    case OpCode::amoand_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, mem & src2);
        m_pIntRegFile->WriteInt32(rd, mem);
        break;
    case OpCode::amoor_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, mem | src2);
        m_pIntRegFile->WriteInt32(rd, mem);
        break;
    case OpCode::amomax_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, std::max(mem, src2));
        m_pIntRegFile->WriteInt32(rd, mem);
        break;
    case OpCode::amomin_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, std::min(mem, src2));
        m_pIntRegFile->WriteInt32(rd, mem);
        break;
    case OpCode::amomaxu_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, std::max(static_cast<uint32_t>(mem), static_cast<uint32_t>(src2)));
        m_pIntRegFile->WriteInt32(rd, mem);
        break;
    case OpCode::amominu_w:
        mem = m_pMemAccessUnit->LoadInt32(src1);
        m_pMemAccessUnit->StoreInt32(src1, std::min(static_cast<uint32_t>(mem), static_cast<uint32_t>(src2)));
        m_pIntRegFile->WriteInt32(rd, mem);
        break;
    default:
        Error(op);
    }
}

void Executor::ProcessRV32F(const Op& op)
{
    switch (op.opCode)
    {
    case OpCode::flw:
        ProcessFloatLoad(op);
        return;
    case OpCode::fsw:
        ProcessFloatStore(op);
        return;
    case OpCode::fmadd_s:
    case OpCode::fmsub_s:
    case OpCode::fnmadd_s:
    case OpCode::fnmsub_s:
        ProcessFloatMulAdd(op);
        return;
    case OpCode::fadd_s:
    case OpCode::fsub_s:
    case OpCode::fmul_s:
    case OpCode::fdiv_s:
    case OpCode::fsqrt_s:
    case OpCode::fmin_s:
    case OpCode::fmax_s:
    case OpCode::fcvt_s_w:
    case OpCode::fcvt_s_wu:
        ProcessFloatCompute(op);
        return;
    case OpCode::feq_s:
    case OpCode::flt_s:
    case OpCode::fle_s:
        ProcessFloatCompare(op);
        return;
    case OpCode::fclass_s:
        ProcessFloatClass(op);
        return;
    case OpCode::fmv_x_w:
        ProcessFloatMoveToInt(op);
        return;
    case OpCode::fmv_w_x:
        ProcessFloatMoveToFp(op);
        return;
    case OpCode::fcvt_w_s:
    case OpCode::fcvt_wu_s:
        ProcessFloatConvertToInt(op);
        return;
    case OpCode::fsgnj_s:
    case OpCode::fsgnjn_s:
    case OpCode::fsgnjx_s:
        ProcessFloatConvertToFp(op);
        return;
    default:
        Error(op);
    }
}

void Executor::ProcessRV32D(const Op& op)
{
    switch (op.opCode)
    {
    case OpCode::fld:
        ProcessDoubleLoad(op);
        return;
    case OpCode::fsd:
        ProcessDoubleStore(op);
        return;
    case OpCode::fmadd_d:
    case OpCode::fmsub_d:
    case OpCode::fnmadd_d:
    case OpCode::fnmsub_d:
        ProcessDoubleMulAdd(op);
        return;
    case OpCode::fadd_d:
    case OpCode::fsub_d:
    case OpCode::fmul_d:
    case OpCode::fdiv_d:
    case OpCode::fsqrt_d:
    case OpCode::fmin_d:
    case OpCode::fmax_d:
    case OpCode::fcvt_d_w:
    case OpCode::fcvt_d_wu:
        ProcessDoubleCompute(op);
        return;
    case OpCode::feq_d:
    case OpCode::flt_d:
    case OpCode::fle_d:
        ProcessDoubleCompare(op);
        return;
    case OpCode::fclass_s:
        ProcessFloatClass(op);
        return;
    case OpCode::fcvt_w_s:
    case OpCode::fcvt_wu_s:
        ProcessDoubleConvertToInt(op);
        return;
    case OpCode::fsgnj_s:
    case OpCode::fsgnjn_s:
    case OpCode::fsgnjx_s:
        ProcessDoubleConvertToFp(op);
        return;
    default:
        Error(op);
    }
}

void Executor::ProcessLui(const Op& op)
{
    const auto& operand = std::get<OperandU>(op.operand);

    m_pIntRegFile->WriteInt32(operand.rd, operand.imm);
}

void Executor::ProcessAuipc(const Op& op, int32_t pc)
{
    const auto& operand = std::get<OperandU>(op.operand);

    m_pIntRegFile->WriteInt32(operand.rd, pc + operand.imm);
}

void Executor::ProcessJal(const Op& op, int32_t pc)
{
    const auto& operand = std::get<OperandJ>(op.operand);

    m_pIntRegFile->WriteInt32(operand.rd, pc + 4);
    m_pCsr->SetProgramCounter(pc + operand.imm);
}

void Executor::ProcessJalr(const Op& op, int32_t pc)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto src = m_pIntRegFile->ReadInt32(operand.rs1);

    m_pIntRegFile->WriteInt32(operand.rd, pc + 4);
    m_pCsr->SetProgramCounter(src + operand.imm);
}

void Executor::ProcessBranch(const Op& op, int32_t pc)
{
    const auto& operand = std::get<OperandB>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadInt32(operand.rs2);

    const auto src1_u = static_cast<uint32_t>(src1);
    const auto src2_u = static_cast<uint32_t>(src2);

    bool jump;

    switch (op.opCode)
    {
    case OpCode::beq:
        jump = (src1 == src2);
        break;
    case OpCode::bne:
        jump = (src1 != src2);
        break;
    case OpCode::blt:
        jump = (src1 < src2);
        break;
    case OpCode::bge:
        jump = (src1 >= src2);
        break;
    case OpCode::bltu:
        jump = (src1_u < src2_u);
        break;
    case OpCode::bgeu:
        jump = (src1_u >= src2_u);
        break;
    default:
        Error(op);
    }

    if (jump)
    {
        m_pCsr->SetProgramCounter(pc + operand.imm);
    }
}

void Executor::ProcessLoad(const Op& op)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto address = m_pIntRegFile->ReadInt32(operand.rs1) + operand.imm;

    int32_t value;

    switch (op.opCode)
    {
    case OpCode::lb:
        value = m_pMemAccessUnit->LoadInt8(address);
        break;
    case OpCode::lh:
        value = m_pMemAccessUnit->LoadInt16(address);
        break;
    case OpCode::lw:
        value = m_pMemAccessUnit->LoadInt32(address);
        break;
    case OpCode::lbu:
        value = m_pMemAccessUnit->LoadInt8(address) & 0x000000ff;
        break;
    case OpCode::lhu:
        value = m_pMemAccessUnit->LoadInt16(address) & 0x0000ffff;
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt32(operand.rd, value);
}

void Executor::ProcessStore(const Op& op)
{
    const auto& operand = std::get<OperandS>(op.operand);

    const auto address = m_pIntRegFile->ReadInt32(operand.rs1) + operand.imm;
    const auto value = m_pIntRegFile->ReadInt32(operand.rs2);

    switch (op.opCode)
    {
    case OpCode::sb:
        m_pMemAccessUnit->StoreInt8(address, static_cast<int8_t>(value));
        break;
    case OpCode::sh:
        m_pMemAccessUnit->StoreInt16(address, static_cast<int16_t>(value));
        break;
    case OpCode::sw:
        m_pMemAccessUnit->StoreInt32(address, static_cast<int32_t>(value));
        break;
    default:
        Error(op);
    }
}

void Executor::ProcessAlu(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadInt32(operand.rs2);

    const auto src1_u = m_pIntRegFile->ReadUInt32(operand.rs1);
    const auto src2_u = m_pIntRegFile->ReadUInt32(operand.rs1);

    int32_t value;

    switch (op.opCode)
    {
    case OpCode::add:
        value = src1 + src2;
        break;
    case OpCode::sub:
        value = src1 - src2;
        break;
    case OpCode::slt:
        value = (src1 < src2) ? 1 : 0;
        break;
    case OpCode::sltu:
        value = (src1_u < src2_u) ? 1 : 0;
        break;
    case OpCode::xor_:
        value = src1 ^ src2;
        break;
    case OpCode::or_:
        value = src1 | src2;
        break;
    case OpCode::and_:
        value = src1 & src2;
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt32(operand.rd, value);
}

void Executor::ProcessAluImm(const Op& op)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src1_u = m_pIntRegFile->ReadUInt32(operand.rs1);

    int32_t value;

    switch (op.opCode)
    {
    case OpCode::addi:
        value = src1 + operand.imm;
        break;
    case OpCode::slti:
        value = (src1 < operand.imm) ? 1 : 0;
        break;
    case OpCode::sltiu:
        value = (src1_u < static_cast<uint32_t>(operand.imm)) ? 1 : 0;
        break;
    case OpCode::xori:
        value = src1 ^ operand.imm;
        break;
    case OpCode::ori:
        value = src1 | operand.imm;
        break;
    case OpCode::andi:
        value = src1 & operand.imm;
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt32(operand.rd, value);
}

void Executor::ProcessShift(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadInt32(operand.rs2);

    const auto src1_u = m_pIntRegFile->ReadUInt32(operand.rs1);;

    int32_t value;

    switch (op.opCode)
    {
    case OpCode::sll:
        value = src1 << src2;
        break;
    case OpCode::srl:
        value = src1_u >> src2;
        break;
    case OpCode::sra:
        value = src1 >> src2;
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt32(operand.rd, value);
}

void Executor::ProcessShiftImm(const Op& op)
{
    const auto& operand = std::get<OperandShiftImm>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src1_u = m_pIntRegFile->ReadUInt32(operand.rs1);

    int32_t value;

    switch (op.opCode)
    {
    case OpCode::slli:
        value = src1 << operand.shamt;
        break;
    case OpCode::srli:
        value = src1_u >> operand.shamt;
        break;
    case OpCode::srai:
        value = src1 >> operand.shamt;
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt32(operand.rd, value);
}

void Executor::ProcessPriv(const Op& op)
{
    switch (op.opCode)
    {
    case OpCode::ecall:
        break;
    case OpCode::ebreak:
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
        m_pCsr->SetHaltFlag(true);
        break;
    default:
        Error(op);
    }
}

void Executor::ProcessCsr(const Op& op)
{
    const auto& operand = std::get<OperandCsr>(op.operand);

    const auto srcCsr = m_pCsrAccessor->Read(static_cast<int>(operand.csr));
    const auto srcIntReg = m_pIntRegFile->ReadInt32(operand.rs1);

    switch (op.opCode)
    {
    case OpCode::csrrw:
        m_pCsrAccessor->Write(static_cast<int>(operand.csr), srcIntReg);
        break;
    case OpCode::csrrs:
        m_pCsrAccessor->Write(static_cast<int>(operand.csr), srcCsr | srcIntReg);
        break;
    case OpCode::csrrc:
        m_pCsrAccessor->Write(static_cast<int>(operand.csr), srcCsr & ~srcIntReg);
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt32(operand.rd, srcCsr);
}

void Executor::ProcessCsrImm(const Op& op)
{
    const auto& operand = std::get<OperandCsrImm>(op.operand);
    const auto srcCsr = m_pCsrAccessor->Read(static_cast<int>(operand.csr));

    switch (op.opCode)
    {
    case OpCode::csrrwi:
        m_pCsrAccessor->Write(static_cast<int>(operand.csr), operand.zimm);
        break;
    case OpCode::csrrsi:
        m_pCsrAccessor->Write(static_cast<int>(operand.csr), srcCsr | operand.zimm);
        break;
    case OpCode::csrrci:
        m_pCsrAccessor->Write(static_cast<int>(operand.csr), srcCsr & ~operand.zimm);
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt32(operand.rd, srcCsr);
}

void Executor::ProcessFloatLoad(const Op& op)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto address = m_pIntRegFile->ReadInt32(operand.rs1) + operand.imm;
    const auto value = static_cast<uint32_t>(m_pMemAccessUnit->LoadInt32(address));

    m_pFpRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessFloatStore(const Op& op)
{
    const auto& operand = std::get<OperandS>(op.operand);

    const auto address = m_pIntRegFile->ReadInt32(operand.rs1) + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt32(operand.rs2);

    m_pMemAccessUnit->StoreInt32(address, static_cast<int32_t>(value));
}

void Executor::ProcessFloatMulAdd(const Op& op)
{
    const auto& operand = std::get<OperandR4>(op.operand);

    const auto src1 = m_pFpRegFile->ReadFloat(operand.rs1);
    const auto src2 = m_pFpRegFile->ReadFloat(operand.rs2);
    const auto src3 = m_pFpRegFile->ReadFloat(operand.rs3);

    int roundMode = operand.funct3;
    if (roundMode == 7)
    {
        roundMode = m_pCsr->ReadFpCsr().GetMember<fcsr_t::RM>();
    }

    ScopedFpRound scopedFpRound(roundMode);

    float value;

    switch (op.opCode)
    {
    case OpCode::fmadd_s:
        value = src1 * src2 + src3;
        break;
    case OpCode::fmsub_s:
        value = src1 * src2 - src3;
        break;
    case OpCode::fnmadd_s:
        value = - src1 * src2 + src3;
        break;
    case OpCode::fnmsub_s:
        value = - src1 * src2 - src3;
        break;
    default:
        Error(op);
    }

    m_pFpRegFile->WriteFloat(operand.rd, value);
}

void Executor::ProcessFloatCompute(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto fpSrc1 = m_pFpRegFile->ReadFloat(operand.rs1);
    const auto fpSrc2 = m_pFpRegFile->ReadFloat(operand.rs2);

    const auto intSrc1 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto uintSrc1 = m_pIntRegFile->ReadUInt32(operand.rs2);

    const auto srcSign1 = m_pFpRegFile->ReadUInt32(operand.rs1) >> 31;
    const auto srcSign2 = m_pFpRegFile->ReadUInt32(operand.rs2) >> 31;

    int roundMode = operand.funct3;
    if (roundMode == 7)
    {
        roundMode = m_pCsr->ReadFpCsr().GetMember<fcsr_t::RM>();
    }

    ScopedFpRound scopedFpRound(roundMode);

    std::feclearexcept(FE_ALL_EXCEPT);

    fexcept_t except;
    float value;

    switch (op.opCode)
    {
    case OpCode::fadd_s:
        value = fpSrc1 + fpSrc2;
        std::fegetexceptflag(&except, FE_ALL_EXCEPT);

        if (std::isinf(fpSrc1) && std::isinf(fpSrc2) && srcSign1 != srcSign2)
        {
            value = std::numeric_limits<float>::quiet_NaN();
        }
        break;
    case OpCode::fsub_s:
        value = fpSrc1 - fpSrc2;
        std::fegetexceptflag(&except, FE_ALL_EXCEPT);

        if (std::isinf(fpSrc1) && std::isinf(fpSrc2) && srcSign1 == srcSign2)
        {
            value = std::numeric_limits<float>::quiet_NaN();
        }
        break;
    case OpCode::fmul_s:
        value = fpSrc1 * fpSrc2;
        std::fegetexceptflag(&except, FE_ALL_EXCEPT);
        break;
    case OpCode::fdiv_s:
        value = fpSrc1 / fpSrc2;
        std::fegetexceptflag(&except, FE_ALL_EXCEPT);
        break;
    case OpCode::fsqrt_s:
        value = std::sqrt(fpSrc1);
        std::fegetexceptflag(&except, FE_ALL_EXCEPT);
        break;
    case OpCode::fmin_s:
        value = std::min(fpSrc1, fpSrc2);
        std::fegetexceptflag(&except, FE_ALL_EXCEPT);
        break;
    case OpCode::fmax_s:
        value = std::max(fpSrc1, fpSrc2);
        std::fegetexceptflag(&except, FE_ALL_EXCEPT);
        break;
    case OpCode::fcvt_s_w:
        value = static_cast<float>(intSrc1);
        std::fegetexceptflag(&except, FE_ALL_EXCEPT);
        break;
    case OpCode::fcvt_s_wu:
        value = static_cast<float>(uintSrc1);
        std::fegetexceptflag(&except, FE_ALL_EXCEPT);
        break;
    default:
        Error(op);
    }

    UpdateFpCsr(except);

    m_pFpRegFile->WriteFloat(operand.rd, value);
}

void Executor::ProcessFloatCompare(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pFpRegFile->ReadFloat(operand.rs1);
    const auto src2 = m_pFpRegFile->ReadFloat(operand.rs2);

    const auto src1_u32 = m_pFpRegFile->ReadUInt32(operand.rs1);
    const auto src2_u32 = m_pFpRegFile->ReadUInt32(operand.rs2);

    fexcept_t except;
    int32_t value;

    switch (op.opCode)
    {
    case OpCode::feq_s:
        if (IsSignalingNan(src1_u32) || IsSignalingNan(src2_u32))
        {
            except = FE_INVALID;
        }
        else
        {
            except = 0;
        }

        value = (src1 == src2) ? 1 : 0;
        break;
    case OpCode::flt_s:
        if (std::isnan(src1) || std::isnan(src2))
        {
            except = FE_INVALID;
        }
        else
        {
            except = 0;
        }

        value = (src1 < src2) ? 1 : 0;
        break;
    case OpCode::fle_s:
        if (std::isnan(src1) || std::isnan(src2))
        {
            except = FE_INVALID;
        }
        else
        {
            except = 0;
        }

        value = (src1 <= src2) ? 1 : 0;
        break;
    default:
        Error(op);
    }

    UpdateFpCsr(except);

    m_pIntRegFile->WriteInt32(operand.rd, value);
}

void Executor::ProcessFloatClass(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src = m_pFpRegFile->ReadUInt32(operand.rs1);

    const auto value = GetRvFpClass(src);

    m_pIntRegFile->WriteInt32(operand.rd, static_cast<int32_t>(value));
}

void Executor::ProcessFloatMoveToInt(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto value = m_pFpRegFile->ReadUInt32(operand.rs1);

    m_pIntRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessFloatMoveToFp(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto value = m_pIntRegFile->ReadUInt32(operand.rs1);

    m_pFpRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessFloatConvertToInt(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src = m_pFpRegFile->ReadFloat(operand.rs1);

    std::feclearexcept(FE_ALL_EXCEPT);

    switch (op.opCode)
    {
    case OpCode::fcvt_w_s:
        m_pIntRegFile->WriteInt32(operand.rd, static_cast<int32_t>(src));
        break;
    case OpCode::fcvt_wu_s:
        m_pIntRegFile->WriteUInt32(operand.rd, static_cast<uint32_t>(src));
        break;
    default:
        Error(op);
    }

    fexcept_t except;
    std::fegetexceptflag(&except, FE_ALL_EXCEPT);

    UpdateFpCsr(except);
}

void Executor::ProcessFloatConvertToFp(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pFpRegFile->ReadUInt32(operand.rs1);
    const auto src2 = m_pFpRegFile->ReadUInt32(operand.rs2);

    uint32_t value;

    std::feclearexcept(FE_ALL_EXCEPT);

    switch (op.opCode)
    {
    case OpCode::fsgnj_s:
        value = (src1 & 0x7fffffff) | (src2 & 0x80000000);
        break;
    case OpCode::fsgnjn_s:
        value = (src1 & 0x7fffffff) | ((~src2) & 0x80000000);
        break;
    case OpCode::fsgnjx_s:
        value = src1 ^ (src2 & 0x80000000);
        break;
    default:
        Error(op);
    }

    fexcept_t except;
    std::fegetexceptflag(&except, FE_ALL_EXCEPT);

    UpdateFpCsr(except);

    m_pFpRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessDoubleLoad(const Op& op)
{
    Error(op);
}

void Executor::ProcessDoubleStore(const Op& op)
{
    Error(op);
}

void Executor::ProcessDoubleMulAdd(const Op& op)
{
    Error(op);
}

void Executor::ProcessDoubleCompute(const Op& op)
{
    Error(op);
}

void Executor::ProcessDoubleCompare(const Op& op)
{
    Error(op);
}

void Executor::ProcessDoubleConvertToInt(const Op& op)
{
    Error(op);
}

void Executor::ProcessDoubleConvertToFp(const Op& op)
{
    Error(op);
}

void Executor::UpdateFpCsr(const fexcept_t& value)
{
    auto fpCsr = m_pCsr->ReadFpCsr();

    fpCsr.SetMember<fcsr_t::AE>(GetRvFpExceptFlags(value));

    m_pCsr->WriteFpCsr(fpCsr);
}

[[noreturn]]
void Executor::Error(const Op& op)
{
    char opDescription[1024];
    SNPrintOp(opDescription, sizeof(opDescription), op);

    fprintf(stderr, "[Executor::Error] Unable to handle Op: %s\n", opDescription);

    std::exit(1);
}

}}}
