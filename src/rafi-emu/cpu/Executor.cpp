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
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>

#include <boost/multiprecision/cpp_int.hpp>

#include <rafi/emu.h>
#include <rafi/fp.h>

#include "Executor.h"

#pragma fenv_access (on)

namespace mp = boost::multiprecision;

namespace rafi { namespace emu { namespace cpu {

std::optional<Trap> Executor::PreCheckTrap(const Op& op, vaddr_t pc, uint32_t insn) const
{
    switch (op.opClass)
    {
    case OpClass::RV32C:
        return PreCheckTrapRV32C(op, pc);
    case OpClass::RV64C:
        return PreCheckTrapRV64C(op, pc);
    case OpClass::RV32F:
    case OpClass::RV32D:
    case OpClass::RV64F:
    case OpClass::RV64D:
        if (!IsFpEnabled())
        {
            return MakeIllegalInstructionException(pc, insn);
        }
        break;
    }

    switch (op.opCode)
    {
    case OpCode::lb:
    case OpCode::lh:
    case OpCode::lw:
    case OpCode::lbu:
    case OpCode::lhu:
    case OpCode::lwu:
    case OpCode::ld:
    case OpCode::flw:
    case OpCode::fld:
        if (IsRV32(op.opClass))
        {
            return PreCheckTrapRV32_Load(op, pc);
        }
        else if (IsRV64(op.opClass))
        {
            return PreCheckTrapRV64_Load(op, pc);
        }
        else
        {
            RAFI_NOT_IMPLEMENTED();
        }
    case OpCode::sb:
    case OpCode::sh:
    case OpCode::sw:
    case OpCode::sd:
    case OpCode::fsw:
    case OpCode::fsd:
        if (IsRV32(op.opClass))
        {
            return PreCheckTrapRV32_Store(op, pc);
        }
        else if (IsRV64(op.opClass))
        {
            return PreCheckTrapRV64_Store(op, pc);
        }
        else
        {
            RAFI_NOT_IMPLEMENTED();
        }
    case OpCode::csrrw:
    case OpCode::csrrs:
    case OpCode::csrrc:
        return PreCheckTrap_Csr(op, pc, insn);
    case OpCode::csrrwi:
    case OpCode::csrrsi:
    case OpCode::csrrci:
        return PreCheckTrap_CsrImm(op, pc, insn);
    case OpCode::wfi:
        return PreCheckTrap_Wfi(pc, insn);
    case OpCode::sfence_vma:
        return PreCheckTrap_Fence(pc, insn);
    case OpCode::lr_d:
    case OpCode::lr_w:
        if (IsRV32(op.opClass))
        {
            return PreCheckTrapRV32_LoadReserved(op, pc);
        }
        else if (IsRV64(op.opClass))
        {
            return PreCheckTrapRV64_LoadReserved(op, pc);
        }
        else
        {
            RAFI_NOT_IMPLEMENTED();
        }
    case OpCode::sc_d:
    case OpCode::sc_w:
        if (IsRV32(op.opClass))
        {
            return PreCheckTrapRV32_StoreConditional(op, pc);
        }
        else if (IsRV64(op.opClass))
        {
            return PreCheckTrapRV64_StoreConditional(op, pc);
        }
        else
        {
            RAFI_NOT_IMPLEMENTED();
        }
    case OpCode::amoswap_d:
    case OpCode::amoswap_w:
    case OpCode::amoadd_d:
    case OpCode::amoadd_w:
    case OpCode::amoxor_d:
    case OpCode::amoxor_w:
    case OpCode::amoand_d:
    case OpCode::amoand_w:
    case OpCode::amoor_d:
    case OpCode::amoor_w:
    case OpCode::amomin_d:
    case OpCode::amomin_w:
    case OpCode::amomax_d:
    case OpCode::amomax_w:
    case OpCode::amominu_d:
    case OpCode::amominu_w:
    case OpCode::amomaxu_d:
    case OpCode::amomaxu_w:
        if (IsRV32(op.opClass))
        {
            return PreCheckTrapRV32_Atomic(op, pc);
        }
        else if (IsRV64(op.opClass))
        {
            return PreCheckTrapRV64_Atomic(op, pc);
        }
        else
        {
            RAFI_NOT_IMPLEMENTED();
        }
    default:
        return std::nullopt;
    }
}

std::optional<Trap> Executor::PostCheckTrap(const Op& op, vaddr_t pc) const
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

void Executor::ProcessOp(const Op& op, vaddr_t pc)
{
    switch (op.opClass)
    {
    case OpClass::RV32I:
        ProcessRV32I(op, pc);
        break;
    case OpClass::RV32M:
        ProcessRV32M(op);
        break;
    case OpClass::RV32C:
        ProcessRV32C(op, pc);
        break;
    case OpClass::RV64I:
        ProcessRV64I(op, pc);
        break;
    case OpClass::RV64M:
        ProcessRV64M(op);
        break;
    case OpClass::RV64C:
        ProcessRV64C(op, pc);
        break;
    case OpClass::RV32A:
    case OpClass::RV64A:
        ProcessRVA(op);
        break;
    case OpClass::RV32F:
    case OpClass::RV64F:
        ProcessRVF(op);
        break;
    case OpClass::RV32D:
    case OpClass::RV64D:
        ProcessRVD(op);
        break;
    default:
        Error(op);
    }
}

std::optional<Trap> Executor::PreCheckTrapRV32C(const Op& op, vaddr_t pc) const
{
    switch (op.opCode)
    {
    case OpCode::c_fld:
        return PreCheckTrapRV32C_FLD(op, pc);
    case OpCode::c_fldsp:
        return PreCheckTrapRV32C_FLDSP(op, pc);
    case OpCode::c_flw:
        return PreCheckTrapRV32C_FLW(op, pc);
    case OpCode::c_flwsp:
        return PreCheckTrapRV32C_FLWSP(op, pc);
    case OpCode::c_fsd:
        return PreCheckTrapRV32C_FSD(op, pc);
    case OpCode::c_fsdsp:
        return PreCheckTrapRV32C_FSDSP(op, pc);
    case OpCode::c_fsw:
        return PreCheckTrapRV32C_FSW(op, pc);
    case OpCode::c_fswsp:
        return PreCheckTrapRV32C_FSWSP(op, pc);
    case OpCode::c_lw:
        return PreCheckTrapRV32C_LW(op, pc);
    case OpCode::c_lwsp:
        return PreCheckTrapRV32C_LWSP(op, pc);
    case OpCode::c_sw:
        return PreCheckTrapRV32C_SW(op, pc);
    case OpCode::c_swsp:
        return PreCheckTrapRV32C_SWSP(op, pc);
    default:
        return std::nullopt;
    }
}

std::optional<Trap> Executor::PreCheckTrapRV64C(const Op& op, vaddr_t pc) const
{
    switch (op.opCode)
    {
    case OpCode::c_fld:
        return PreCheckTrapRV64C_FLD(op, pc);
    case OpCode::c_fldsp:
        return PreCheckTrapRV64C_FLDSP(op, pc);
    case OpCode::c_fsd:
        return PreCheckTrapRV64C_FSD(op, pc);
    case OpCode::c_fsdsp:
        return PreCheckTrapRV64C_FSDSP(op, pc);
    case OpCode::c_ld:
        return PreCheckTrapRV64C_LD(op, pc);
    case OpCode::c_ldsp:
        return PreCheckTrapRV64C_LDSP(op, pc);
    case OpCode::c_lw:
        return PreCheckTrapRV64C_LW(op, pc);
    case OpCode::c_lwsp:
        return PreCheckTrapRV64C_LWSP(op, pc);
    case OpCode::c_sd:
        return PreCheckTrapRV64C_SD(op, pc);
    case OpCode::c_sdsp:
        return PreCheckTrapRV64C_SDSP(op, pc);
    case OpCode::c_sw:
        return PreCheckTrapRV64C_SW(op, pc);
    case OpCode::c_swsp:
        return PreCheckTrapRV64C_SWSP(op, pc);
    default:
        return std::nullopt;
    }
}

std::optional<Trap> Executor::PreCheckTrapRV32_Load(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandI>(op.operand);
    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32_LoadReserved(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandR>(op.operand);
    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1);

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32_Store(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandS>(op.operand);
    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32_StoreConditional(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandR>(op.operand);
    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1);

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32_Atomic(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandR>(op.operand);
    const vaddr_t address = m_pIntRegFile->ReadUInt32(operand.rs1);

    const auto trap = m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
    if (trap)
    {
        return trap;
    }

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64_Load(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandI>(op.operand);
    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64_LoadReserved(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandR>(op.operand);
    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1);

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64_Store(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandS>(op.operand);
    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64_StoreConditional(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandR>(op.operand);
    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1);

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64_Atomic(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandR>(op.operand);
    const vaddr_t address = m_pIntRegFile->ReadUInt64(operand.rs1);

    const auto trap = m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
    if (trap)
    {
        return trap;
    }

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrap_Csr(const Op& op, vaddr_t pc, uint32_t insn) const
{
    const auto& operand = std::get<OperandCsr>(op.operand);

    const bool write =
        (op.opCode == OpCode::csrrs && operand.rs1 != 0) ||
        (op.opCode == OpCode::csrrc && operand.rs1 != 0) ||
        (op.opCode == OpCode::csrrw);

    return m_pCsr->CheckTrap(operand.csr, write, pc, insn);
}

std::optional<Trap> Executor::PreCheckTrap_CsrImm(const Op& op, vaddr_t pc, uint32_t insn) const
{
    const auto& operand = std::get<OperandCsrImm>(op.operand);

    const bool write =
        (op.opCode == OpCode::csrrs && operand.zimm != 0) ||
        (op.opCode == OpCode::csrrc && operand.zimm != 0) ||
        (op.opCode == OpCode::csrrw);

    return m_pCsr->CheckTrap(operand.csr, write, pc, insn);
}

std::optional<Trap> Executor::PreCheckTrap_Wfi(vaddr_t pc, uint32_t insn) const
{
    const auto priv = m_pCsr->GetPrivilegeLevel();
    const auto status = m_pCsr->ReadStatus();

    if (priv == PrivilegeLevel::User || (priv == PrivilegeLevel::Supervisor && status.GetMember<xstatus_t::TW>()))
    {
        return MakeIllegalInstructionException(pc, insn);
    }
    else
    {
        return std::nullopt;
    }
}

std::optional<Trap> Executor::PreCheckTrap_Fence(vaddr_t pc, uint32_t insn) const
{
    const auto priv = m_pCsr->GetPrivilegeLevel();
    const auto status = m_pCsr->ReadStatus();

    if (priv == PrivilegeLevel::User || (priv == PrivilegeLevel::Supervisor && status.GetMember<xstatus_t::TVM>()))
    {
        return MakeIllegalInstructionException(pc, insn);
    }
    else
    {
        return std::nullopt;
    }
}

std::optional<Trap> Executor::PreCheckTrapRV32C_FLD(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCL>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32C_FLDSP(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32C_FLW(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCL>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32C_FLWSP(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32C_FSD(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt64(operand.rs2);

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32C_FSDSP(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCSS>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt64(operand.rs2);

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32C_FSW(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt32(operand.rs2);

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32C_FSWSP(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCSS>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt32(operand.rs2);

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}


std::optional<Trap> Executor::PreCheckTrapRV32C_LW(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCL>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32C_LWSP(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32C_SW(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV32C_SWSP(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCSS>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64C_FLD(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCL>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64C_FLDSP(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto address = ReadStackPointer64() + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64C_FSD(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64C_FSDSP(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCSS>(op.operand);

    const auto address = ReadStackPointer64() + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64C_LD(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCL>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64C_LDSP(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto address = ReadStackPointer64() + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64C_LW(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCL>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64C_LWSP(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto address = ReadStackPointer64() + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Load, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64C_SD(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64C_SDSP(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCSS>(op.operand);

    const auto address = ReadStackPointer64() + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64C_SW(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

std::optional<Trap> Executor::PreCheckTrapRV64C_SWSP(const Op& op, vaddr_t pc) const
{
    const auto& operand = std::get<OperandCSS>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;

    return m_pMemAccessUnit->CheckTrap(MemoryAccessType::Store, pc, address);
}

// PostCheckTrap
std::optional<Trap> Executor::PostCheckTrapForEcall(vaddr_t pc) const
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
        RAFI_EMU_NOT_IMPLEMENTED();
    }
}

void Executor::ProcessRV32I(const Op& op, vaddr_t pc)
{
    switch (op.opCode)
    {
    case OpCode::lui:
        ProcessRV32I_Lui(op);
        return;
    case OpCode::auipc:
        ProcessRV32I_Auipc(op, pc);
        return;
    case OpCode::jal:
        ProcessRV32I_Jal(op, pc);
        return;
    case OpCode::jalr:
        ProcessRV32I_Jalr(op, pc);
        return;
    case OpCode::beq:
    case OpCode::bne:
    case OpCode::blt:
    case OpCode::bge:
    case OpCode::bltu:
    case OpCode::bgeu:
        ProcessRV32I_Branch(op, pc);
        return;
    case OpCode::lb:
    case OpCode::lh:
    case OpCode::lw:
    case OpCode::lbu:
    case OpCode::lhu:
        ProcessRV32I_Load(op);
        return;
    case OpCode::sb:
    case OpCode::sh:
    case OpCode::sw:
        ProcessRV32I_Store(op);
        return;
    case OpCode::addi:
    case OpCode::slti:
    case OpCode::sltiu:
    case OpCode::xori:
    case OpCode::ori:
    case OpCode::andi:
        ProcessRV32I_AluImm(op);
        return;
    case OpCode::sll:
    case OpCode::srl:
    case OpCode::sra:
        ProcessRV32I_Shift(op);
        return;
    case OpCode::slli:
    case OpCode::srli:
    case OpCode::srai:
        ProcessRV32I_ShiftImm(op);
        return;
    case OpCode::add:
    case OpCode::sub:
    case OpCode::slt:
    case OpCode::sltu:
    case OpCode::xor_:
    case OpCode::or_:
    case OpCode::and_:
        ProcessRV32I_Alu(op);
        return;
    case OpCode::ecall:
    case OpCode::ebreak:
    case OpCode::mret:
    case OpCode::sret:
    case OpCode::uret:
    case OpCode::wfi:
        ProcessRV32I_Priv(op);
        return;
    case OpCode::fence:
    case OpCode::fence_i:
    case OpCode::sfence_vma:
        // Do nothing for memory fence instructions.
        return;
    case OpCode::csrrw:
    case OpCode::csrrs:
    case OpCode::csrrc:
        ProcessRV32I_Csr(op);
        return;
    case OpCode::csrrwi:
    case OpCode::csrrsi:
    case OpCode::csrrci:
        ProcessRV32I_CsrImm(op);
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

    const uint32_t src1_u = m_pIntRegFile->ReadUInt32(rs1);
    const uint32_t src2_u = m_pIntRegFile->ReadUInt32(rs2);

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

void Executor::ProcessRVA(const Op& op)
{
    switch (op.opCode)
    {
    case OpCode::lr_w:
        ProcessRVA_Load32(op);
        break;
    case OpCode::lr_d:
        ProcessRVA_Load64(op);
        break;
    case OpCode::sc_w:
        ProcessRVA_Store32(op);
        break;
    case OpCode::sc_d:
        ProcessRVA_Store64(op);
        break;
    case OpCode::amoswap_w:
    case OpCode::amoadd_w:
    case OpCode::amoxor_w:
    case OpCode::amoand_w:
    case OpCode::amoor_w:
    case OpCode::amomax_w:
    case OpCode::amomin_w:
    case OpCode::amomaxu_w:
    case OpCode::amominu_w:
        ProcessRVA_Atomic32(op);
        break;
    case OpCode::amoswap_d:
    case OpCode::amoadd_d:
    case OpCode::amoxor_d:
    case OpCode::amoand_d:
    case OpCode::amoor_d:
    case OpCode::amomax_d:
    case OpCode::amomin_d:
    case OpCode::amomaxu_d:
    case OpCode::amominu_d:
        ProcessRVA_Atomic64(op);
        break;
    default:
        Error(op);
    }
}

void Executor::ProcessRVF(const Op& op)
{
    switch (op.opCode)
    {
    case OpCode::flw:
        if (op.opClass == OpClass::RV32F)
        {
            ProcessRV32F_Load(op);
        }
        else if (op.opClass == OpClass::RV64F)
        {
            ProcessRV64F_Load(op);
        }
        else
        {
            RAFI_EMU_NOT_IMPLEMENTED();
        }
        return;
    case OpCode::fsw:
        if (op.opClass == OpClass::RV32F)
        {
            ProcessRV32F_Store(op);
        }
        else if (op.opClass == OpClass::RV64F)
        {
            ProcessRV64F_Store(op);
        }
        else
        {
            RAFI_EMU_NOT_IMPLEMENTED();
        }
        return;
    case OpCode::fmadd_s:
    case OpCode::fmsub_s:
    case OpCode::fnmadd_s:
    case OpCode::fnmsub_s:
        ProcessRVF_MulAdd(op);
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
    case OpCode::fcvt_s_l:
    case OpCode::fcvt_s_lu:
        ProcessRVF_Compute(op);
        return;
    case OpCode::feq_s:
    case OpCode::flt_s:
    case OpCode::fle_s:
        ProcessRVF_Compare(op);
        return;
    case OpCode::fclass_s:
        ProcessRVF_Class(op);
        return;
    case OpCode::fmv_x_w:
        ProcessRVF_MoveToInt(op);
        return;
    case OpCode::fmv_w_x:
        ProcessRVF_MoveToFp(op);
        return;
    case OpCode::fcvt_w_s:
    case OpCode::fcvt_wu_s:
        ProcessRVF_ConvertToInt32(op);
        return;
    case OpCode::fcvt_l_s:
    case OpCode::fcvt_lu_s:
        ProcessRVF_ConvertToInt64(op);
        return;
    case OpCode::fsgnj_s:
    case OpCode::fsgnjn_s:
    case OpCode::fsgnjx_s:
        ProcessRVF_ConvertSign(op);
        return;
    default:
        Error(op);
    }
}

void Executor::ProcessRVD(const Op& op)
{
    switch (op.opCode)
    {
    case OpCode::fld:
        if (op.opClass == OpClass::RV32D)
        {
            ProcessRV32D_Load(op);
        }
        else if (op.opClass == OpClass::RV64D)
        {
            ProcessRV64D_Load(op);
        }
        else
        {
            RAFI_EMU_NOT_IMPLEMENTED();
        }
        return;
    case OpCode::fsd:
        if (op.opClass == OpClass::RV32D)
        {
            ProcessRV32D_Store(op);
        }
        else if (op.opClass == OpClass::RV64D)
        {
            ProcessRV64D_Store(op);
        }
        else
        {
            RAFI_EMU_NOT_IMPLEMENTED();
        }
        return;
    case OpCode::fmadd_d:
    case OpCode::fmsub_d:
    case OpCode::fnmadd_d:
    case OpCode::fnmsub_d:
        ProcessRVD_MulAdd(op);
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
    case OpCode::fcvt_d_l:
    case OpCode::fcvt_d_lu:
        ProcessRVD_Compute(op);
        return;
    case OpCode::feq_d:
    case OpCode::flt_d:
    case OpCode::fle_d:
        ProcessRVD_Compare(op);
        return;
    case OpCode::fclass_d:
        ProcessRVD_Class(op);
        return;
    case OpCode::fmv_x_d:
        ProcessRVD_MoveToInt(op);
        return;
    case OpCode::fmv_d_x:
        ProcessRVD_MoveToFp(op);
        return;
    case OpCode::fcvt_w_d:
    case OpCode::fcvt_wu_d:
        ProcessRVD_ConvertToInt32(op);
        return;
    case OpCode::fcvt_l_d:
    case OpCode::fcvt_lu_d:
        ProcessRVD_ConvertToInt64(op);
        return;
    case OpCode::fcvt_s_d:
        ProcessRVD_ConvertFp64ToFp32(op);
        return;
    case OpCode::fcvt_d_s:
        ProcessRVD_ConvertFp32ToFp64(op);
        return;
    case OpCode::fsgnj_d:
    case OpCode::fsgnjn_d:
    case OpCode::fsgnjx_d:
        ProcessRVD_ConvertSign(op);
        return;
    default:
        Error(op);
    }
}

void Executor::ProcessRV32C(const Op& op, vaddr_t pc)
{
    switch (op.opCode)
    {
    case OpCode::c_nop:
        return;
    case OpCode::c_mv:
    case OpCode::c_add:
    case OpCode::c_sub:
    case OpCode::c_and:
    case OpCode::c_or:
    case OpCode::c_xor:
        ProcessRV32C_Alu(op);
        return;
    case OpCode::c_li:
    case OpCode::c_lui:
    case OpCode::c_addi:
    case OpCode::c_andi:
    case OpCode::c_srli:
    case OpCode::c_srai:
    case OpCode::c_slli:
        ProcessRV32C_AluImm(op);
        return;
    case OpCode::c_beqz:
    case OpCode::c_bnez:
        ProcessRV32C_Branch(op, pc);
        return;
    case OpCode::c_addi4spn:
        ProcessRV32C_ADDI4SPN(op);
        return;
    case OpCode::c_addi16sp:
        ProcessRV32C_ADDI16SP(op);
        return;
    case OpCode::c_fld:
        ProcessRV32C_FLD(op);
        return;
    case OpCode::c_fldsp:
        ProcessRV32C_FLDSP(op);
        return;
    case OpCode::c_flw:
        ProcessRV32C_FLW(op);
        return;
    case OpCode::c_flwsp:
        ProcessRV32C_FLWSP(op);
        return;
    case OpCode::c_fsd:
        ProcessRV32C_FSD(op);
        return;
    case OpCode::c_fsdsp:
        ProcessRV32C_FSDSP(op);
        return;
    case OpCode::c_fsw:
        ProcessRV32C_FSW(op);
        return;
    case OpCode::c_fswsp:
        ProcessRV32C_FSWSP(op);
        return;
    case OpCode::c_j:
        ProcessRV32C_J(op, pc);
        return;
    case OpCode::c_jal:
        ProcessRV32C_JAL(op, pc);
        return;
    case OpCode::c_jr:
        ProcessRV32C_JR(op);
        return;
    case OpCode::c_jalr:
        ProcessRV32C_JALR(op, pc);
        return;
    case OpCode::c_lw:
        ProcessRV32C_LW(op);
        return;
    case OpCode::c_lwsp:
        ProcessRV32C_LWSP(op);
        return;
    case OpCode::c_sw:
        ProcessRV32C_SW(op);
        return;
    case OpCode::c_swsp:
        ProcessRV32C_SWSP(op);
        return;
    default:
        Error(op);
    }
}

void Executor::ProcessRV64I(const Op& op, vaddr_t pc)
{
    switch (op.opCode)
    {
    case OpCode::lui:
        ProcessRV64I_Lui(op);
        return;
    case OpCode::auipc:
        ProcessRV64I_Auipc(op, pc);
        return;
    case OpCode::jal:
        ProcessRV64I_Jal(op, pc);
        return;
    case OpCode::jalr:
        ProcessRV64I_Jalr(op, pc);
        return;
    case OpCode::beq:
    case OpCode::bne:
    case OpCode::blt:
    case OpCode::bge:
    case OpCode::bltu:
    case OpCode::bgeu:
        ProcessRV64I_Branch(op, pc);
        return;
    case OpCode::lb:
    case OpCode::lh:
    case OpCode::lw:
    case OpCode::ld:
    case OpCode::lbu:
    case OpCode::lhu:
    case OpCode::lwu:
        ProcessRV64I_Load(op);
        return;
    case OpCode::sb:
    case OpCode::sh:
    case OpCode::sw:
    case OpCode::sd:
        ProcessRV64I_Store(op);
        return;
    case OpCode::addi:
    case OpCode::addiw:
    case OpCode::slti:
    case OpCode::sltiu:
    case OpCode::xori:
    case OpCode::ori:
    case OpCode::andi:
        ProcessRV64I_AluImm(op);
        return;
    case OpCode::sll:
    case OpCode::sllw:
    case OpCode::srl:
    case OpCode::srlw:
    case OpCode::sra:
    case OpCode::sraw:
        ProcessRV64I_Shift(op);
        return;
    case OpCode::slli:
    case OpCode::slliw:
    case OpCode::srli:
    case OpCode::srliw:
    case OpCode::srai:
    case OpCode::sraiw:
        ProcessRV64I_ShiftImm(op);
        return;
    case OpCode::add:
    case OpCode::addw:
    case OpCode::sub:
    case OpCode::subw:
    case OpCode::slt:
    case OpCode::sltu:
    case OpCode::xor_:
    case OpCode::or_:
    case OpCode::and_:
        ProcessRV64I_Alu(op);
        return;
    case OpCode::ecall:
    case OpCode::ebreak:
    case OpCode::mret:
    case OpCode::sret:
    case OpCode::uret:
    case OpCode::wfi:
        ProcessRV64I_Priv(op);
        return;
    case OpCode::fence:
    case OpCode::fence_i:
    case OpCode::sfence_vma:
        // Do nothing for memory fence instructions.
        return;
    case OpCode::csrrw:
    case OpCode::csrrs:
    case OpCode::csrrc:
        ProcessRV64I_Csr(op);
        return;
    case OpCode::csrrwi:
    case OpCode::csrrsi:
    case OpCode::csrrci:
        ProcessRV64I_CsrImm(op);
        return;
    default:
        Error(op);
    }
}

void Executor::ProcessRV64M(const Op& op)
{
    const auto operand = std::get<OperandR>(op.operand);

    const auto src1_u32 = m_pIntRegFile->ReadUInt32(operand.rs1);
    const auto src2_u32 = m_pIntRegFile->ReadUInt32(operand.rs2);

    const auto src1_s32 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src2_s32 = m_pIntRegFile->ReadInt32(operand.rs2);

    const auto src1_s64 = m_pIntRegFile->ReadInt64(operand.rs1);
    const auto src2_s64 = m_pIntRegFile->ReadInt64(operand.rs2);

    const auto src1_u64 = m_pIntRegFile->ReadUInt64(operand.rs1);
    const auto src2_u64 = m_pIntRegFile->ReadUInt64(operand.rs2);

    const auto src1_s128 = mp::int128_t(src1_s64);
    const auto src2_s128 = mp::int128_t(src2_s64);

    const auto src1_u128 = mp::uint128_t(src1_u64);
    const auto src2_u128 = mp::uint128_t(src2_u64);

    int64_t value;

    switch (op.opCode)
    {
    case OpCode::mul:
        value = src1_s64 * src2_s64;
        break;
    case OpCode::mulh:
        value = static_cast<int64_t>((src1_s128 * src2_s128) >> 64);
        break;
    case OpCode::mulhsu:
        value = static_cast<int64_t>((src1_s128 * src2_u128) >> 64);
        break;
    case OpCode::mulhu:
        value = static_cast<int64_t>(static_cast<uint64_t>((src1_u128 * src2_u128) >> 64));
        break;
    case OpCode::mulw:
        value = SignExtend<int64_t>(32, src1_s32 * src2_s32);
        break;
    case OpCode::div:
        if (src1_u64 == (1ull << 63) && src2_s64 == -1ll)
        {
            value = static_cast<int64_t>(1ull << 63);
        }
        else if (src2_s64 == 0)
        {
            value = -1ll;
        }
        else
        {
            value = src1_s64 / src2_s64;
        }
        break;
    case OpCode::divu:
        if (src2_s64 == 0)
        {
            value = -1ll;
        }
        else
        {
            value = static_cast<int64_t>(src1_u64 / src2_u64);
        }
        break;
    case OpCode::divuw:
        if (src2_u32 == 0)
        {
            value = -1ll;
        }
        else
        {
            value = SignExtend<int64_t>(32, src1_u32 / src2_u32);
        }
        break;
    case OpCode::divw:
        if (src1_u32 == (1ull << 31) && src2_s32 == -1)
        {
            value = SignExtend<int64_t>(32, 1ull << 31);
        }
        else if (src2_s64 == 0)
        {
            value = -1ll;
        }
        else
        {
            value = SignExtend<int64_t>(32, src1_s32 / src2_s32);
        }
        break;
    case OpCode::rem:
        if (src1_u64 == 0x80000000 && src2_s64 == -1)
        {
            value = 0;
        }
        else if (src2_s64 == 0)
        {
            value = src1_s64;
        }
        else
        {
            value = src1_s64 % src2_s64;
        }
        break;
    case OpCode::remu:
        if (src2_s64 == 0)
        {
            value = src1_s64;
        }
        else
        {
            value = static_cast<int64_t>(src1_u64 % src2_u64);
        }
        break;
    case OpCode::remuw:
        if (src2_u32 == 0)
        {
            value = SignExtend<int64_t>(32, src1_s32);
        }
        else
        {
            value = SignExtend<int64_t>(32, src1_u32 % src2_u32);
        }
        break;
    case OpCode::remw:
        if (src1_u32 == (1ull << 31) && src2_s32 == -1)
        {
            value = 0;
        }
        else if (src2_s32 == 0)
        {
            value = SignExtend<int64_t>(32, src1_s32);
        }
        else
        {
            value = SignExtend<int64_t>(32, src1_s32 % src2_s32);
        }
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRV64C(const Op& op, vaddr_t pc)
{
    switch (op.opCode)
    {
    case OpCode::c_nop:
        return;
    case OpCode::c_mv:
    case OpCode::c_add:
    case OpCode::c_addw:
    case OpCode::c_sub:
    case OpCode::c_subw:
    case OpCode::c_and:
    case OpCode::c_or:
    case OpCode::c_xor:
        ProcessRV64C_Alu(op);
        return;
    case OpCode::c_li:
    case OpCode::c_lui:
    case OpCode::c_addi:
    case OpCode::c_addiw:
    case OpCode::c_andi:
    case OpCode::c_srli:
    case OpCode::c_srai:
    case OpCode::c_slli:
        ProcessRV64C_AluImm(op);
        return;
    case OpCode::c_beqz:
    case OpCode::c_bnez:
        ProcessRV64C_Branch(op, pc);
        return;
    case OpCode::c_addi4spn:
        ProcessRV64C_ADDI4SPN(op);
        return;
    case OpCode::c_addi16sp:
        ProcessRV64C_ADDI16SP(op);
        return;
    case OpCode::c_fld:
        ProcessRV64C_FLD(op);
        return;
    case OpCode::c_fldsp:
        ProcessRV64C_FLDSP(op);
        return;
    case OpCode::c_fsd:
        ProcessRV64C_FSD(op);
        return;
    case OpCode::c_fsdsp:
        ProcessRV64C_FSDSP(op);
        return;
    case OpCode::c_j:
        ProcessRV64C_J(op, pc);
        return;
    case OpCode::c_jr:
        ProcessRV64C_JR(op);
        return;
    case OpCode::c_jalr:
        ProcessRV64C_JALR(op, pc);
        return;
    case OpCode::c_ld:
        ProcessRV64C_LD(op);
        return;
    case OpCode::c_ldsp:
        ProcessRV64C_LDSP(op);
        return;
    case OpCode::c_lw:
        ProcessRV64C_LW(op);
        return;
    case OpCode::c_lwsp:
        ProcessRV64C_LWSP(op);
        return;
    case OpCode::c_sd:
        ProcessRV64C_SD(op);
        return;
    case OpCode::c_sdsp:
        ProcessRV64C_SDSP(op);
        return;
    case OpCode::c_sw:
        ProcessRV64C_SW(op);
        return;
    case OpCode::c_swsp:
        ProcessRV64C_SWSP(op);
        return;
    default:
        Error(op);
    }
}

void Executor::ProcessRV32I_Lui(const Op& op)
{
    const auto& operand = std::get<OperandU>(op.operand);

    m_pIntRegFile->WriteInt32(operand.rd, operand.imm);
}

void Executor::ProcessRV32I_Auipc(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandU>(op.operand);

    m_pIntRegFile->WriteInt32(operand.rd, pc + operand.imm);
}

void Executor::ProcessRV32I_Jal(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandJ>(op.operand);

    m_pIntRegFile->WriteInt32(operand.rd, pc + 4);
    m_pCsr->SetProgramCounter(pc + operand.imm);
}

void Executor::ProcessRV32I_Jalr(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto src = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto address = (~0x1) & (src + operand.imm);

    m_pIntRegFile->WriteInt32(operand.rd, pc + 4);
    m_pCsr->SetProgramCounter(address);
}

void Executor::ProcessRV32I_Branch(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandB>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadInt32(operand.rs2);

    const auto src1_u = m_pIntRegFile->ReadUInt32(operand.rs1);
    const auto src2_u = m_pIntRegFile->ReadUInt32(operand.rs2);

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

void Executor::ProcessRV32I_Load(const Op& op)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;

    uint32_t value;

    switch (op.opCode)
    {
    case OpCode::lb:
        value = static_cast<int8_t>(m_pMemAccessUnit->LoadUInt8(address));
        break;
    case OpCode::lh:
        value = static_cast<int16_t>(m_pMemAccessUnit->LoadUInt16(address));
        break;
    case OpCode::lw:
        value = static_cast<int32_t>(m_pMemAccessUnit->LoadUInt32(address));
        break;
    case OpCode::lbu:
        value = m_pMemAccessUnit->LoadUInt8(address);
        break;
    case OpCode::lhu:
        value = m_pMemAccessUnit->LoadUInt16(address);
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessRV32I_Store(const Op& op)
{
    const auto& operand = std::get<OperandS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pIntRegFile->ReadUInt32(operand.rs2);

    switch (op.opCode)
    {
    case OpCode::sb:
        m_pMemAccessUnit->StoreUInt8(address, static_cast<uint8_t>(value));
        break;
    case OpCode::sh:
        m_pMemAccessUnit->StoreUInt16(address, static_cast<uint16_t>(value));
        break;
    case OpCode::sw:
        m_pMemAccessUnit->StoreUInt32(address, static_cast<uint32_t>(value));
        break;
    default:
        Error(op);
    }
}

void Executor::ProcessRV32I_Alu(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadInt32(operand.rs2);

    const auto src1_u = m_pIntRegFile->ReadUInt32(operand.rs1);
    const auto src2_u = m_pIntRegFile->ReadUInt32(operand.rs2);

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

void Executor::ProcessRV32I_AluImm(const Op& op)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto imm = static_cast<int32_t>(operand.imm);
    const auto imm_u = static_cast<uint32_t>(operand.imm);

    const auto src1 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src1_u = m_pIntRegFile->ReadUInt32(operand.rs1);

    int32_t value;

    switch (op.opCode)
    {
    case OpCode::addi:
        value = src1 + imm;
        break;
    case OpCode::slti:
        value = (src1 < imm) ? 1 : 0;
        break;
    case OpCode::sltiu:
        value = (src1_u < imm_u) ? 1 : 0;
        break;
    case OpCode::xori:
        value = src1 ^ imm;
        break;
    case OpCode::ori:
        value = src1 | imm;
        break;
    case OpCode::andi:
        value = src1 & imm;
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt32(operand.rd, value);
}

void Executor::ProcessRV32I_Shift(const Op& op)
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

void Executor::ProcessRV32I_ShiftImm(const Op& op)
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

void Executor::ProcessRV32I_Priv(const Op& op)
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
        break;
    default:
        Error(op);
    }
}

void Executor::ProcessRV32I_Csr(const Op& op)
{
    const auto& operand = std::get<OperandCsr>(op.operand);

    const auto srcCsr = m_pCsr->ReadUInt32(operand.csr);
    const auto srcIntReg = m_pIntRegFile->ReadInt32(operand.rs1);

    switch (op.opCode)
    {
    case OpCode::csrrw:
        m_pCsr->WriteUInt32(operand.csr, srcIntReg);
        break;
    case OpCode::csrrs:
        m_pCsr->WriteUInt32(operand.csr, srcCsr | srcIntReg);
        break;
    case OpCode::csrrc:
        m_pCsr->WriteUInt32(operand.csr, srcCsr & ~srcIntReg);
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt32(operand.rd, srcCsr);
}

void Executor::ProcessRV32I_CsrImm(const Op& op)
{
    const auto& operand = std::get<OperandCsrImm>(op.operand);
    const auto srcCsr = m_pCsr->ReadUInt32(operand.csr);

    switch (op.opCode)
    {
    case OpCode::csrrwi:
        m_pCsr->WriteUInt32(operand.csr, operand.zimm);
        break;
    case OpCode::csrrsi:
        m_pCsr->WriteUInt32(operand.csr, srcCsr | operand.zimm);
        break;
    case OpCode::csrrci:
        m_pCsr->WriteUInt32(operand.csr, srcCsr & ~operand.zimm);
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt32(operand.rd, srcCsr);
}

void Executor::ProcessRV64I_Lui(const Op& op)
{
    const auto& operand = std::get<OperandU>(op.operand);

    m_pIntRegFile->WriteInt64(operand.rd, operand.imm);
}

void Executor::ProcessRV64I_Auipc(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandU>(op.operand);

    m_pIntRegFile->WriteInt64(operand.rd, pc + operand.imm);
}

void Executor::ProcessRV64I_Jal(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandJ>(op.operand);

    m_pIntRegFile->WriteInt64(operand.rd, pc + 4);
    m_pCsr->SetProgramCounter(pc + operand.imm);
}

void Executor::ProcessRV64I_Jalr(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto src = m_pIntRegFile->ReadInt64(operand.rs1);
    const auto address = (~0x1) & (src + operand.imm);

    m_pIntRegFile->WriteInt64(operand.rd, pc + 4);
    m_pCsr->SetProgramCounter(address);
}

void Executor::ProcessRV64I_Branch(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandB>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt64(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadInt64(operand.rs2);

    const auto src1_u = m_pIntRegFile->ReadUInt64(operand.rs1);
    const auto src2_u = m_pIntRegFile->ReadUInt64(operand.rs2);

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

void Executor::ProcessRV64I_Load(const Op& op)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;

    uint64_t value;

    switch (op.opCode)
    {
    case OpCode::lb:
        value = SignExtend<uint64_t>(8, m_pMemAccessUnit->LoadUInt8(address));
        break;
    case OpCode::lh:
        value = SignExtend<uint64_t>(16, m_pMemAccessUnit->LoadUInt16(address));
        break;
    case OpCode::lw:
        value = SignExtend<uint64_t>(32, m_pMemAccessUnit->LoadUInt32(address));
        break;
    case OpCode::ld:
        value = SignExtend<uint64_t>(64, m_pMemAccessUnit->LoadUInt64(address));
        break;
    case OpCode::lbu:
        value = ZeroExtend<uint64_t>(8, m_pMemAccessUnit->LoadUInt8(address));
        break;
    case OpCode::lhu:
        value = ZeroExtend<uint64_t>(16, m_pMemAccessUnit->LoadUInt16(address));
        break;
    case OpCode::lwu:
        value = ZeroExtend<uint64_t>(32, m_pMemAccessUnit->LoadUInt32(address));
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRV64I_Store(const Op& op)
{
    const auto& operand = std::get<OperandS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;
    const auto value = m_pIntRegFile->ReadUInt64(operand.rs2);

    switch (op.opCode)
    {
    case OpCode::sb:
        m_pMemAccessUnit->StoreUInt8(address, static_cast<uint8_t>(value));
        break;
    case OpCode::sh:
        m_pMemAccessUnit->StoreUInt16(address, static_cast<uint16_t>(value));
        break;
    case OpCode::sw:
        m_pMemAccessUnit->StoreUInt32(address, static_cast<uint32_t>(value));
        break;
    case OpCode::sd:
        m_pMemAccessUnit->StoreUInt64(address, static_cast<uint64_t>(value));
        break;
    default:
        Error(op);
    }
}

void Executor::ProcessRV64I_Alu(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt64(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadInt64(operand.rs2);

    const auto src1_u = m_pIntRegFile->ReadUInt64(operand.rs1);
    const auto src2_u = m_pIntRegFile->ReadUInt64(operand.rs2);

    const auto src1_s32 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src2_s32 = m_pIntRegFile->ReadInt32(operand.rs2);

    int64_t value;

    switch (op.opCode)
    {
    case OpCode::add:
        value = src1 + src2;
        break;
    case OpCode::addw:
        value = SignExtend<int64_t>(32, src1_s32 + src2_s32);
        break;
    case OpCode::sub:
        value = src1 - src2;
        break;
    case OpCode::subw:
        value = SignExtend<int64_t>(32, src1_s32 - src2_s32);
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

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRV64I_AluImm(const Op& op)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto imm = static_cast<int64_t>(operand.imm);
    const auto imm_u = static_cast<uint64_t>(operand.imm);
    const auto imm_s32 = static_cast<int32_t>(operand.imm);

    const auto src1 = m_pIntRegFile->ReadInt64(operand.rs1);
    const auto src1_u = m_pIntRegFile->ReadUInt64(operand.rs1);
    const auto src1_s32 = m_pIntRegFile->ReadInt32(operand.rs1);

    int64_t value;

    switch (op.opCode)
    {
    case OpCode::addi:
        value = src1 + imm;
        break;
    case OpCode::addiw:
        value = SignExtend<int64_t>(32, src1_s32 + imm_s32);
        break;
    case OpCode::slti:
        value = (src1 < imm) ? 1 : 0;
        break;
    case OpCode::sltiu:
        value = (src1_u < imm_u) ? 1 : 0;
        break;
    case OpCode::xori:
        value = src1 ^ imm;
        break;
    case OpCode::ori:
        value = src1 | imm;
        break;
    case OpCode::andi:
        value = src1 & imm;
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRV64I_Shift(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt64(operand.rs1);
    const auto src1_u = m_pIntRegFile->ReadUInt64(operand.rs1);

    const auto src1_s32 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src1_u32 = m_pIntRegFile->ReadUInt32(operand.rs1);

    const int shamt = m_pIntRegFile->ReadInt64(operand.rs2) & 0x3f;

    int64_t value;

    switch (op.opCode)
    {
    case OpCode::sll:
        value = src1 << shamt;
        break;
    case OpCode::sllw:
        value = SignExtend<int64_t>(32, src1_s32 << shamt);
        break;
    case OpCode::srl:
        value = src1_u >> shamt;
        break;
    case OpCode::srlw:
        value = SignExtend<int64_t>(32, src1_u32 >> shamt);
        break;
    case OpCode::sra:
        value = src1 >> shamt;
        break;
    case OpCode::sraw:
        value = SignExtend<int64_t>(32, src1_s32 >> shamt);
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRV64I_ShiftImm(const Op& op)
{
    const auto& operand = std::get<OperandShiftImm>(op.operand);

    const auto shamt_s32 = static_cast<int32_t>(operand.shamt);
    const auto shamt_u32 = static_cast<uint32_t>(operand.shamt);
    const auto shamt_s64 = static_cast<int64_t>(operand.shamt);
    const auto shamt_u64 = static_cast<uint64_t>(operand.shamt);

    const auto src1 = m_pIntRegFile->ReadInt64(operand.rs1);
    const auto src1_u = m_pIntRegFile->ReadUInt64(operand.rs1);

    const auto src1_s32 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src1_u32 = m_pIntRegFile->ReadUInt32(operand.rs1);

    int64_t value;

    switch (op.opCode)
    {
    case OpCode::slli:
        value = src1 << shamt_s64;
        break;
    case OpCode::slliw:
        value = SignExtend<int64_t>(32, src1_s32 << shamt_s32);
        break;
    case OpCode::srli:
        value = src1_u >> shamt_u64;
        break;
    case OpCode::srliw:
        value = SignExtend<int64_t>(32, src1_u32 >> shamt_u32);
        break;
    case OpCode::srai:
        value = src1 >> shamt_s64;
        break;
    case OpCode::sraiw:
        value = SignExtend<int64_t>(32, src1_s32 >> shamt_s32);
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRV64I_Priv(const Op& op)
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
        break;
    default:
        Error(op);
    }
}

void Executor::ProcessRV64I_Csr(const Op& op)
{
    const auto& operand = std::get<OperandCsr>(op.operand);

    const auto srcCsr = m_pCsr->ReadUInt64(operand.csr);
    const auto srcIntReg = m_pIntRegFile->ReadInt64(operand.rs1);

    switch (op.opCode)
    {
    case OpCode::csrrw:
        m_pCsr->WriteUInt64(operand.csr, srcIntReg);
        break;
    case OpCode::csrrs:
        m_pCsr->WriteUInt64(operand.csr, srcCsr | srcIntReg);
        break;
    case OpCode::csrrc:
        m_pCsr->WriteUInt64(operand.csr, srcCsr & ~srcIntReg);
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt64(operand.rd, srcCsr);
}

void Executor::ProcessRV64I_CsrImm(const Op& op)
{
    const auto& operand = std::get<OperandCsrImm>(op.operand);
    const auto srcCsr = m_pCsr->ReadUInt64(operand.csr);

    switch (op.opCode)
    {
    case OpCode::csrrwi:
        m_pCsr->WriteUInt64(operand.csr, operand.zimm);
        break;
    case OpCode::csrrsi:
        m_pCsr->WriteUInt64(operand.csr, srcCsr | operand.zimm);
        break;
    case OpCode::csrrci:
        m_pCsr->WriteUInt64(operand.csr, srcCsr & ~operand.zimm);
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt64(operand.rd, srcCsr);
}


void Executor::ProcessRV32C_Alu(const Op& op)
{
    const auto& operand = std::get<OperandCR>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadInt32(operand.rs2);

    int32_t result;

    switch (op.opCode)
    {
    case OpCode::c_mv:
        result = src2;
        break;
    case OpCode::c_add:
        result = src1 + src2;
        break;
    case OpCode::c_sub:
        result = src1 - src2;
        break;
    case OpCode::c_and:
        result = src1 & src2;
        break;
    case OpCode::c_or:
        result = src1 | src2;
        break;
    case OpCode::c_xor:
        result = src1 ^ src2;
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt32(operand.rd, result);
}

void Executor::ProcessRV32C_AluImm(const Op& op)
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src1_u = m_pIntRegFile->ReadUInt32(operand.rs1);

    int32_t result;

    switch (op.opCode)
    {
    case OpCode::c_li:
    case OpCode::c_lui:
        result = operand.imm;
        break;
    case OpCode::c_addi:
        result = src1 + operand.imm;
        break;
    case OpCode::c_andi:
        result = src1 & operand.imm;
        break;
    case OpCode::c_srli:
        result = static_cast<int32_t>(src1_u >> operand.imm);
        break;
    case OpCode::c_srai:
        result = src1 >> operand.imm;
        break;
    case OpCode::c_slli:
        result = src1 << operand.imm;
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt32(operand.rd, result);
}

void Executor::ProcessRV32C_Branch(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandCB>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt32(operand.rs1);

    bool cond;

    switch (op.opCode)
    {
    case OpCode::c_beqz:
        cond = (src1 == 0);
        break;
    case OpCode::c_bnez:
        cond = (src1 != 0);
        break;
    default:
        Error(op);
    }

    if (cond)
    {
        m_pCsr->SetProgramCounter(pc + operand.imm);
    }
}

void Executor::ProcessRV32C_ADDI4SPN(const Op& op)
{
    const auto& operand = std::get<OperandCIW>(op.operand);

    const auto result = ReadStackPointer32() + operand.imm;

    m_pIntRegFile->WriteUInt32(operand.rd, result);
}

void Executor::ProcessRV32C_ADDI16SP(const Op& op)
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto result = ReadStackPointer32() + operand.imm;

    m_pIntRegFile->WriteUInt32(operand.rd, result);
}

void Executor::ProcessRV32C_FLD(const Op& op)
{
    const auto& operand = std::get<OperandCL>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt64(address);

    m_pFpRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRV32C_FLDSP(const Op& op)
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt64(address);

    m_pFpRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRV32C_FLW(const Op& op)
{
    const auto& operand = std::get<OperandCL>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt32(address);

    m_pFpRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessRV32C_FLWSP(const Op& op)
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt32(address);

    m_pFpRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessRV32C_FSD(const Op& op)
{
    const auto& operand = std::get<OperandCS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt64(operand.rs2);

    m_pMemAccessUnit->StoreUInt64(address, value);
}

void Executor::ProcessRV32C_FSDSP(const Op& op)
{
    const auto& operand = std::get<OperandCSS>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt64(operand.rs2);

    m_pMemAccessUnit->StoreUInt64(address, value);
}

void Executor::ProcessRV32C_FSW(const Op& op)
{
    const auto& operand = std::get<OperandCS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt32(operand.rs2);

    m_pMemAccessUnit->StoreUInt32(address, value);
}

void Executor::ProcessRV32C_FSWSP(const Op& op)
{
    const auto& operand = std::get<OperandCSS>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt32(operand.rs2);

    m_pMemAccessUnit->StoreUInt32(address, value);
}

void Executor::ProcessRV32C_J(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandCJ>(op.operand);

    m_pCsr->SetProgramCounter(pc + operand.imm);
}

void Executor::ProcessRV32C_JAL(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandCJ>(op.operand);

    m_pCsr->SetProgramCounter(pc + operand.imm);

    WriteLinkRegister32(pc + 2);
}

void Executor::ProcessRV32C_JR(const Op& op)
{
    const auto& operand = std::get<OperandCR>(op.operand);

    const auto src = m_pIntRegFile->ReadUInt32(operand.rs1);
    const auto address = (~0x1) & src;

    m_pCsr->SetProgramCounter(address);
}

void Executor::ProcessRV32C_JALR(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandCR>(op.operand);

    const auto src = m_pIntRegFile->ReadUInt32(operand.rs1);
    const auto address = (~0x1) & src;

    m_pCsr->SetProgramCounter(address);
    WriteLinkRegister32(pc + 2);
}

void Executor::ProcessRV32C_LW(const Op& op)
{
    const auto& operand = std::get<OperandCL>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt32(address);

    m_pIntRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessRV32C_LWSP(const Op& op)
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt32(address);

    m_pIntRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessRV32C_SW(const Op& op)
{
    const auto& operand = std::get<OperandCS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pIntRegFile->ReadUInt32(operand.rs2);

    m_pMemAccessUnit->StoreUInt32(address, value);
}

void Executor::ProcessRV32C_SWSP(const Op& op)
{
    const auto& operand = std::get<OperandCSS>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;
    const auto value = m_pIntRegFile->ReadUInt32(operand.rs2);

    m_pMemAccessUnit->StoreUInt32(address, value);
}

void Executor::ProcessRV64C_Alu(const Op& op)
{
    const auto& operand = std::get<OperandCR>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt64(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadInt64(operand.rs2);

    const auto src1_s32 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src2_s32 = m_pIntRegFile->ReadInt32(operand.rs2);

    int64_t value;

    switch (op.opCode)
    {
    case OpCode::c_mv:
        value = src2;
        break;
    case OpCode::c_add:
        value = src1 + src2;
        break;
    case OpCode::c_addw:
        value = SignExtend<int64_t>(32, src1_s32 + src2_s32);
        break;
    case OpCode::c_sub:
        value = src1 - src2;
        break;
    case OpCode::c_subw:
        value = SignExtend<int64_t>(32, src1_s32 - src2_s32);
        break;
    case OpCode::c_and:
        value = src1 & src2;
        break;
    case OpCode::c_or:
        value = src1 | src2;
        break;
    case OpCode::c_xor:
        value = src1 ^ src2;
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRV64C_AluImm(const Op& op)
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt64(operand.rs1);
    const auto src1_u = m_pIntRegFile->ReadUInt64(operand.rs1);
    const auto src1_s32 = m_pIntRegFile->ReadInt32(operand.rs1);

    int64_t value;

    switch (op.opCode)
    {
    case OpCode::c_li:
    case OpCode::c_lui:
        value = operand.imm;
        break;
    case OpCode::c_addi:
        value = src1 + operand.imm;
        break;
    case OpCode::c_addiw:
        value = SignExtend(32, src1_s32 + operand.imm);
        break;
    case OpCode::c_andi:
        value = src1 & operand.imm;
        break;
    case OpCode::c_srli:
        value = static_cast<int64_t>(src1_u >> operand.imm);
        break;
    case OpCode::c_srai:
        value = src1 >> operand.imm;
        break;
    case OpCode::c_slli:
        value = src1 << operand.imm;
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRV64C_Branch(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandCB>(op.operand);

    const auto src1 = m_pIntRegFile->ReadInt64(operand.rs1);

    bool cond;

    switch (op.opCode)
    {
    case OpCode::c_beqz:
        cond = (src1 == 0);
        break;
    case OpCode::c_bnez:
        cond = (src1 != 0);
        break;
    default:
        Error(op);
    }

    if (cond)
    {
        m_pCsr->SetProgramCounter(pc + operand.imm);
    }
}

void Executor::ProcessRV64C_ADDI4SPN(const Op& op)
{
    const auto& operand = std::get<OperandCIW>(op.operand);

    const auto result = ReadStackPointer64() + operand.imm;

    m_pIntRegFile->WriteUInt64(operand.rd, result);
}

void Executor::ProcessRV64C_ADDI16SP(const Op& op)
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto result = ReadStackPointer64() + operand.imm;

    m_pIntRegFile->WriteUInt64(operand.rd, result);
}

void Executor::ProcessRV64C_FLD(const Op& op)
{
    const auto& operand = std::get<OperandCL>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt64(address);

    m_pFpRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRV64C_FLDSP(const Op& op)
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto address = ReadStackPointer64() + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt64(address);

    m_pFpRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRV64C_FSD(const Op& op)
{
    const auto& operand = std::get<OperandCS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt64(operand.rs2);

    m_pMemAccessUnit->StoreUInt64(address, value);
}

void Executor::ProcessRV64C_FSDSP(const Op& op)
{
    const auto& operand = std::get<OperandCSS>(op.operand);

    const auto address = ReadStackPointer64() + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt64(operand.rs2);

    m_pMemAccessUnit->StoreUInt64(address, value);
}

void Executor::ProcessRV64C_J(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandCJ>(op.operand);

    m_pCsr->SetProgramCounter(pc + operand.imm);
}

void Executor::ProcessRV64C_JR(const Op& op)
{
    const auto& operand = std::get<OperandCR>(op.operand);

    const auto src = m_pIntRegFile->ReadUInt64(operand.rs1);
    const auto address = (~0x1) & src;

    m_pCsr->SetProgramCounter(address);
}

void Executor::ProcessRV64C_JALR(const Op& op, vaddr_t pc)
{
    const auto& operand = std::get<OperandCR>(op.operand);

    const auto src = m_pIntRegFile->ReadUInt64(operand.rs1);
    const auto address = (~0x1) & src;

    m_pCsr->SetProgramCounter(address);
    WriteLinkRegister64(pc + 2);
}

void Executor::ProcessRV64C_LD(const Op& op)
{
    const auto& operand = std::get<OperandCL>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt64(address);

    m_pIntRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRV64C_LDSP(const Op& op)
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto address = ReadStackPointer64() + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt64(address);

    m_pIntRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRV64C_LW(const Op& op)
{
    const auto& operand = std::get<OperandCL>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = SignExtend<uint64_t>(32, m_pMemAccessUnit->LoadUInt32(address));

    m_pIntRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRV64C_LWSP(const Op& op)
{
    const auto& operand = std::get<OperandCI>(op.operand);

    const auto address = ReadStackPointer64() + operand.imm;
    const auto value = SignExtend<uint64_t>(32, m_pMemAccessUnit->LoadUInt32(address));

    m_pIntRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRV64C_SD(const Op& op)
{
    const auto& operand = std::get<OperandCS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;
    const auto value = m_pIntRegFile->ReadUInt64(operand.rs2);

    m_pMemAccessUnit->StoreUInt64(address, value);
}

void Executor::ProcessRV64C_SDSP(const Op& op)
{
    const auto& operand = std::get<OperandCSS>(op.operand);

    const auto address = ReadStackPointer64() + operand.imm;
    const auto value = m_pIntRegFile->ReadUInt64(operand.rs2);

    m_pMemAccessUnit->StoreUInt64(address, value);
}

void Executor::ProcessRV64C_SW(const Op& op)
{
    const auto& operand = std::get<OperandCS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pIntRegFile->ReadUInt32(operand.rs2);

    m_pMemAccessUnit->StoreUInt32(address, value);
}

void Executor::ProcessRV64C_SWSP(const Op& op)
{
    const auto& operand = std::get<OperandCSS>(op.operand);

    const auto address = ReadStackPointer32() + operand.imm;
    const auto value = m_pIntRegFile->ReadUInt32(operand.rs2);

    m_pMemAccessUnit->StoreUInt32(address, value);
}

void Executor::ProcessRVA_Atomic32(const Op& op)
{
    const auto operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pIntRegFile->ReadUInt32(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadUInt32(operand.rs2);

    auto value = m_pMemAccessUnit->LoadUInt32(src1);

    switch (op.opCode)
    {
    case OpCode::amoswap_w:
        m_pMemAccessUnit->StoreUInt32(src1, src2);
        break;
    case OpCode::amoadd_w:
        m_pMemAccessUnit->StoreUInt32(src1, value + src2);
        break;
    case OpCode::amoxor_w:
        m_pMemAccessUnit->StoreUInt32(src1, value ^ src2);
        break;
    case OpCode::amoand_w:
        m_pMemAccessUnit->StoreUInt32(src1, value & src2);
        break;
    case OpCode::amoor_w:
        m_pMemAccessUnit->StoreUInt32(src1, value | src2);
        break;
    case OpCode::amomax_w:
        m_pMemAccessUnit->StoreUInt32(src1, std::max(static_cast<int32_t>(value), static_cast<int32_t>(src2)));
        break;
    case OpCode::amomin_w:
        m_pMemAccessUnit->StoreUInt32(src1, std::min(static_cast<int32_t>(value), static_cast<int32_t>(src2)));
        break;
    case OpCode::amomaxu_w:
        m_pMemAccessUnit->StoreUInt32(src1, std::max(value, src2));
        break;
    case OpCode::amominu_w:
        m_pMemAccessUnit->StoreUInt32(src1, std::min(value, src2));
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteInt64(operand.rd, SignExtend<int64_t>(32, value));
}

void Executor::ProcessRVA_Atomic64(const Op& op)
{
    const auto operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pIntRegFile->ReadUInt64(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadUInt64(operand.rs2);

    auto value = m_pMemAccessUnit->LoadUInt64(src1);

    switch (op.opCode)
    {
    case OpCode::amoswap_d:
        m_pMemAccessUnit->StoreUInt64(src1, src2);
        break;
    case OpCode::amoadd_d:
        m_pMemAccessUnit->StoreUInt64(src1, value + src2);
        break;
    case OpCode::amoxor_d:
        m_pMemAccessUnit->StoreUInt64(src1, value ^ src2);
        break;
    case OpCode::amoand_d:
        m_pMemAccessUnit->StoreUInt64(src1, value & src2);
        break;
    case OpCode::amoor_d:
        m_pMemAccessUnit->StoreUInt64(src1, value | src2);
        break;
    case OpCode::amomax_d:
        m_pMemAccessUnit->StoreUInt64(src1, std::max(static_cast<int64_t>(value), static_cast<int64_t>(src2)));
        break;
    case OpCode::amomin_d:
        m_pMemAccessUnit->StoreUInt64(src1, std::min(static_cast<int64_t>(value), static_cast<int64_t>(src2)));
        break;
    case OpCode::amomaxu_d:
        m_pMemAccessUnit->StoreUInt64(src1, std::max(value, src2));
        break;
    case OpCode::amominu_d:
        m_pMemAccessUnit->StoreUInt64(src1, std::min(value, src2));
        break;
    default:
        Error(op);
    }

    m_pIntRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRVA_Load32(const Op& op)
{
    const auto operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pIntRegFile->ReadUInt32(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadUInt32(operand.rs2);

    m_ReserveAddress = src1;

    const auto value = m_pMemAccessUnit->LoadUInt32(src1);

    m_pIntRegFile->WriteInt32(operand.rd, value);
}

void Executor::ProcessRVA_Load64(const Op& op)
{
    const auto operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pIntRegFile->ReadUInt64(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadUInt64(operand.rs2);

    m_ReserveAddress = src1;

    const auto value = m_pMemAccessUnit->LoadUInt64(src1);

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRVA_Store32(const Op& op)
{
    const auto operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pIntRegFile->ReadUInt32(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadUInt32(operand.rs2);

    if (m_ReserveAddress == src1)
    {
        m_pMemAccessUnit->StoreUInt32(src1, src2);
        m_pIntRegFile->WriteUInt64(operand.rd, 0);
    }
    else
    {
        m_pIntRegFile->WriteUInt64(operand.rd, 1);
    }
}

void Executor::ProcessRVA_Store64(const Op& op)
{
    const auto operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pIntRegFile->ReadUInt64(operand.rs1);
    const auto src2 = m_pIntRegFile->ReadUInt64(operand.rs2);

    if (m_ReserveAddress == src1)
    {
        m_pMemAccessUnit->StoreUInt64(src1, src2);
        m_pIntRegFile->WriteUInt64(operand.rd, 0);
    }
    else
    {
        m_pIntRegFile->WriteUInt64(operand.rd, 1);
    }
}

void Executor::ProcessRVF_MulAdd(const Op& op)
{
    const auto& operand = std::get<OperandR4>(op.operand);

    const auto src1 = fp::UnboxFloat(m_pFpRegFile->ReadUInt64(operand.rs1));
    const auto src2 = fp::UnboxFloat(m_pFpRegFile->ReadUInt64(operand.rs2));
    const auto src3 = fp::UnboxFloat(m_pFpRegFile->ReadUInt64(operand.rs3));

    int roundMode = operand.funct3;
    if (roundMode == 7)
    {
        roundMode = m_pCsr->ReadFpCsr().GetMember<fcsr_t::RM>();
    }

    fp::ScopedFpRound scopedFpRound(roundMode);

    uint32_t result;

    switch (op.opCode)
    {
    case OpCode::fmadd_s:
        result = fp::MulAdd(src1, src2, src3);
        break;
    case OpCode::fmsub_s:
        result = fp::MulSub(src1, src2, src3);
        break;
    case OpCode::fnmadd_s:
        result = fp::NegMulAdd(src1, src2, src3);
        break;
    case OpCode::fnmsub_s:
        result = fp::NegMulSub(src1, src2, src3);
        break;
    default:
        Error(op);
    }

    UpdateFpCsr();
    NotifyFpDirty();
    m_pFpRegFile->WriteUInt32(operand.rd, result);
}

void Executor::ProcessRVF_Compute(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src1 = fp::UnboxFloat(m_pFpRegFile->ReadUInt64(operand.rs1));
    const auto src2 = fp::UnboxFloat(m_pFpRegFile->ReadUInt64(operand.rs2));

    const auto src1_s32 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src1_s64 = m_pIntRegFile->ReadInt64(operand.rs1);
    const auto src1_u32 = m_pIntRegFile->ReadUInt32(operand.rs1);
    const auto src1_u64 = m_pIntRegFile->ReadUInt64(operand.rs1);

    int roundMode = operand.funct3;
    if (roundMode == 7)
    {
        roundMode = m_pCsr->ReadFpCsr().GetMember<fcsr_t::RM>();
    }

    fp::ScopedFpRound scopedFpRound(roundMode);

    uint64_t value;

    switch (op.opCode)
    {
    case OpCode::fadd_s:
        value = fp::Add(src1, src2);
        break;
    case OpCode::fsub_s:
        value = fp::Sub(src1, src2);
        break;
    case OpCode::fmul_s:
        value = fp::Mul(src1, src2);
        break;
    case OpCode::fdiv_s:
        value = fp::Div(src1, src2);
        break;
    case OpCode::fsqrt_s:
        value = fp::Sqrt(src1);
        break;
    case OpCode::fmin_s:
        value = fp::Min(src1, src2);
        break;
    case OpCode::fmax_s:
        value = fp::Max(src1, src2);
        break;
    case OpCode::fcvt_s_w:
        value = fp::Int32ToFloat(src1_s32);
        break;
    case OpCode::fcvt_s_wu:
        value = fp::UInt32ToFloat(src1_u32);
        break;
    case OpCode::fcvt_s_l:
        value = fp::Int64ToFloat(src1_s64);
        break;
    case OpCode::fcvt_s_lu:
        value = fp::UInt64ToFloat(src1_u64);
        break;
    default:
        Error(op);
    }

    UpdateFpCsr();
    NotifyFpDirty();
    m_pFpRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessRVF_Compare(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src1 = fp::UnboxFloat(m_pFpRegFile->ReadUInt64(operand.rs1));
    const auto src2 = fp::UnboxFloat(m_pFpRegFile->ReadUInt64(operand.rs2));

    int64_t value;

    switch (op.opCode)
    {
    case OpCode::feq_s:
        value = fp::Eq(src1, src2) ? 1 : 0;
        break;
    case OpCode::flt_s:
        value = fp::Lt(src1, src2) ? 1 : 0;
        break;
    case OpCode::fle_s:
        value = fp::Le(src1, src2) ? 1 : 0;
        break;
    default:
        Error(op);
    }

    UpdateFpCsr();

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRVF_Class(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src = m_pFpRegFile->ReadUInt32(operand.rs1);

    const auto value = fp::ConvertToRvFpClass(src);

    m_pIntRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRVF_MoveToInt(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src = m_pFpRegFile->ReadUInt32(operand.rs1);

    const auto value = SignExtend<int64_t>(32, src);

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRVF_MoveToFp(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto value = m_pIntRegFile->ReadUInt32(operand.rs1);

    NotifyFpDirty();
    m_pFpRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessRVF_ConvertToInt32(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src = m_pFpRegFile->ReadUInt32(operand.rs1);

    int roundMode = operand.funct3;
    if (roundMode == 7)
    {
        roundMode = m_pCsr->ReadFpCsr().GetMember<fcsr_t::RM>();
    }

    int64_t value;

    switch (op.opCode)
    {
    case OpCode::fcvt_w_s:
        value = SignExtend<int64_t>(32, fp::FloatToInt32(src, roundMode));
        break;
    case OpCode::fcvt_wu_s:
        value = SignExtend<int64_t>(32, fp::FloatToUInt32(src, roundMode));
        break;
    default:
        Error(op);
    }

    UpdateFpCsr();

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRVF_ConvertToInt64(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src = m_pFpRegFile->ReadUInt64(operand.rs1);

    int roundMode = operand.funct3;
    if (roundMode == 7)
    {
        roundMode = m_pCsr->ReadFpCsr().GetMember<fcsr_t::RM>();
    }

    int64_t value;

    switch (op.opCode)
    {
    case OpCode::fcvt_l_s:
        value = fp::FloatToInt64(src, roundMode);
        break;
    case OpCode::fcvt_lu_s:
        value = fp::FloatToUInt64(src, roundMode);
        break;
    default:
        Error(op);
    }

    UpdateFpCsr();

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRVF_ConvertSign(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src1 = fp::UnboxFloat(m_pFpRegFile->ReadUInt64(operand.rs1));
    const auto src2 = fp::UnboxFloat(m_pFpRegFile->ReadUInt64(operand.rs2));

    uint32_t value;

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

    UpdateFpCsr();
    NotifyFpDirty();
    m_pFpRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessRV32F_Load(const Op& op)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt32(address);

    NotifyFpDirty();
    m_pFpRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessRV32F_Store(const Op& op)
{
    const auto& operand = std::get<OperandS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt32(operand.rs2);

    m_pMemAccessUnit->StoreUInt32(address, value);
}

void Executor::ProcessRV64F_Load(const Op& op)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt32(address);

    NotifyFpDirty();
    m_pFpRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessRV64F_Store(const Op& op)
{
    const auto& operand = std::get<OperandS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt32(operand.rs2);

    m_pMemAccessUnit->StoreUInt32(address, value);
}

void Executor::ProcessRVD_MulAdd(const Op& op)
{
    const auto& operand = std::get<OperandR4>(op.operand);

    const auto src1 = m_pFpRegFile->ReadUInt64(operand.rs1);
    const auto src2 = m_pFpRegFile->ReadUInt64(operand.rs2);
    const auto src3 = m_pFpRegFile->ReadUInt64(operand.rs3);

    int roundMode = operand.funct3;
    if (roundMode == 7)
    {
        roundMode = m_pCsr->ReadFpCsr().GetMember<fcsr_t::RM>();
    }

    fp::ScopedFpRound scopedFpRound(roundMode);

    uint64_t value;

    switch (op.opCode)
    {
    case OpCode::fmadd_d:
        value = fp::MulAdd(src1, src2, src3);
        break;
    case OpCode::fmsub_d:
        value = fp::MulSub(src1, src2, src3);
        break;
    case OpCode::fnmadd_d:
        value = fp::NegMulAdd(src1, src2, src3);
        break;
    case OpCode::fnmsub_d:
        value = fp::NegMulSub(src1, src2, src3);
        break;
    default:
        Error(op);
    }

    UpdateFpCsr();
    NotifyFpDirty();
    m_pFpRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRVD_Compute(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pFpRegFile->ReadUInt64(operand.rs1);
    const auto src2 = m_pFpRegFile->ReadUInt64(operand.rs2);

    const auto src_s32 = m_pIntRegFile->ReadInt32(operand.rs1);
    const auto src_s64 = m_pIntRegFile->ReadInt64(operand.rs1);
    const auto src_u32 = m_pIntRegFile->ReadUInt32(operand.rs1);
    const auto src_u64 = m_pIntRegFile->ReadUInt64(operand.rs1);

    int roundMode = operand.funct3;
    if (roundMode == 7)
    {
        roundMode = m_pCsr->ReadFpCsr().GetMember<fcsr_t::RM>();
    }

    fp::ScopedFpRound scopedFpRound(roundMode);

    uint64_t result;

    switch (op.opCode)
    {
    case OpCode::fadd_d:
        result = fp::Add(src1, src2);
        break;
    case OpCode::fsub_d:
        result = fp::Sub(src1, src2);
        break;
    case OpCode::fmul_d:
        result = fp::Mul(src1, src2);
        break;
    case OpCode::fdiv_d:
        result = fp::Div(src1, src2);
        break;
    case OpCode::fsqrt_d:
        result = fp::Sqrt(src1);
        break;
    case OpCode::fmin_d:
        result = fp::Min(src1, src2);
        break;
    case OpCode::fmax_d:
        result = fp::Max(src1, src2);
        break;
    case OpCode::fcvt_d_w:
        result = fp::Int32ToDouble(src_s32);
        break;
    case OpCode::fcvt_d_wu:
        result = fp::UInt32ToDouble(src_u32);
        break;
    case OpCode::fcvt_d_l:
        result = fp::Int64ToDouble(src_s64);
        break;
    case OpCode::fcvt_d_lu:
        result = fp::UInt64ToDouble(src_u64);
        break;
    default:
        Error(op);
    }

    UpdateFpCsr();
    NotifyFpDirty();
    m_pFpRegFile->WriteUInt64(operand.rd, result);
}

void Executor::ProcessRVD_Compare(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pFpRegFile->ReadUInt64(operand.rs1);
    const auto src2 = m_pFpRegFile->ReadUInt64(operand.rs2);

    uint32_t value;

    switch (op.opCode)
    {
    case OpCode::feq_d:
        value = fp::Eq(src1, src2) ? 1 : 0;
        break;
    case OpCode::flt_d:
        value = fp::Lt(src1, src2) ? 1 : 0;
        break;
    case OpCode::fle_d:
        value = fp::Le(src1, src2) ? 1 : 0;
        break;
    default:
        Error(op);
    }

    UpdateFpCsr();

    m_pIntRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRVD_Class(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src = m_pFpRegFile->ReadUInt64(operand.rs1);

    const auto value = fp::ConvertToRvFpClass(src);

    m_pIntRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRVD_MoveToInt(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto value = m_pFpRegFile->ReadUInt64(operand.rs1);

    m_pIntRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRVD_MoveToFp(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto value = m_pIntRegFile->ReadUInt64(operand.rs1);

    NotifyFpDirty();
    m_pFpRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRVD_ConvertToInt32(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src = m_pFpRegFile->ReadUInt64(operand.rs1);

    int roundMode = operand.funct3;
    if (roundMode == 7)
    {
        roundMode = m_pCsr->ReadFpCsr().GetMember<fcsr_t::RM>();
    }

    int64_t value;

    switch (op.opCode)
    {
    case OpCode::fcvt_w_d:
        value = SignExtend<int64_t>(32, fp::DoubleToInt32(src, roundMode));
        break;
    case OpCode::fcvt_wu_d:
        value = SignExtend<int64_t>(32, fp::DoubleToUInt32(src, roundMode));
        break;
    default:
        Error(op);
    }

    UpdateFpCsr();

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRVD_ConvertToInt64(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src = m_pFpRegFile->ReadUInt64(operand.rs1);

    int roundMode = operand.funct3;
    if (roundMode == 7)
    {
        roundMode = m_pCsr->ReadFpCsr().GetMember<fcsr_t::RM>();
    }

    int64_t value;

    switch (op.opCode)
    {
    case OpCode::fcvt_l_d:
        value = fp::DoubleToInt64(src, roundMode);
        break;
    case OpCode::fcvt_lu_d:
        value = static_cast<int64_t>(fp::DoubleToUInt64(src, roundMode));
        break;
    default:
        Error(op);
    }

    UpdateFpCsr();

    m_pIntRegFile->WriteInt64(operand.rd, value);
}

void Executor::ProcessRVD_ConvertFp32ToFp64(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src = m_pFpRegFile->ReadUInt32(operand.rs1);

    const uint64_t value = fp::FloatToDouble(src);

    UpdateFpCsr();
    NotifyFpDirty();
    m_pFpRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRVD_ConvertFp64ToFp32(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src = m_pFpRegFile->ReadUInt64(operand.rs1);

    const uint32_t value = fp::DoubleToFloat(src);

    UpdateFpCsr();
    NotifyFpDirty();
    m_pFpRegFile->WriteUInt32(operand.rd, value);
}

void Executor::ProcessRVD_ConvertSign(const Op& op)
{
    const auto& operand = std::get<OperandR>(op.operand);

    const auto src1 = m_pFpRegFile->ReadUInt64(operand.rs1);
    const auto src2 = m_pFpRegFile->ReadUInt64(operand.rs2);

    uint64_t value;

    switch (op.opCode)
    {
    case OpCode::fsgnj_d:
        value = (src1 & ((1ull << 63) - 1)) | (src2 & (1ull << 63));
        break;
    case OpCode::fsgnjn_d:
        value = (src1 & ((1ull << 63) - 1)) | ((~src2) & (1ull << 63));
        break;
    case OpCode::fsgnjx_d:
        value = src1 ^ (src2 & (1ull << 63));
        break;
    default:
        Error(op);
    }

    UpdateFpCsr();
    NotifyFpDirty();
    m_pFpRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRV32D_Load(const Op& op)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt64(address);

    NotifyFpDirty();
    m_pFpRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRV32D_Store(const Op& op)
{
    const auto& operand = std::get<OperandS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt32(operand.rs1) + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt64(operand.rs2);

    m_pMemAccessUnit->StoreUInt64(address, value);
}

void Executor::ProcessRV64D_Load(const Op& op)
{
    const auto& operand = std::get<OperandI>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;
    const auto value = m_pMemAccessUnit->LoadUInt64(address);

    NotifyFpDirty();
    m_pFpRegFile->WriteUInt64(operand.rd, value);
}

void Executor::ProcessRV64D_Store(const Op& op)
{
    const auto& operand = std::get<OperandS>(op.operand);

    const auto address = m_pIntRegFile->ReadUInt64(operand.rs1) + operand.imm;
    const auto value = m_pFpRegFile->ReadUInt64(operand.rs2);

    m_pMemAccessUnit->StoreUInt64(address, value);
}

uint32_t Executor::ReadStackPointer32() const
{
    return m_pIntRegFile->ReadUInt32(2);
}

uint64_t Executor::ReadStackPointer64() const
{
    return m_pIntRegFile->ReadUInt64(2);
}

void Executor::WriteLinkRegister32(uint32_t value)
{
    return m_pIntRegFile->WriteUInt32(1, value);
}

void Executor::WriteLinkRegister64(uint64_t value)
{
    return m_pIntRegFile->WriteUInt64(1, value);
}

bool Executor::IsFpEnabled() const
{
    return m_pCsr->ReadStatus().GetMember<xstatus_t::FS>() != 0;
}

void Executor::NotifyFpDirty()
{
    auto status = m_pCsr->ReadStatus();

    status.SetMember<xstatus_t::FS>(3);

    m_pCsr->WriteStatus(status);
}

void Executor::UpdateFpCsr()
{
    auto fpCsr = m_pCsr->ReadFpCsr();

    fpCsr.SetMember<fcsr_t::AE>(fp::GetRvExceptionFlags());

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
