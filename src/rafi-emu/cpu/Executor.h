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

#pragma once

#include <cfenv>

#include <rafi/common.h>

#include "AtomicManager.h"
#include "Csr.h"
#include "FpRegFile.h"
#include "IntRegFile.h"
#include "MemoryAccessUnit.h"
#include "Trap.h"
#include "TrapProcessor.h"

namespace rafi { namespace emu { namespace cpu {

class Executor
{
public:
    Executor(AtomicManager* pAtomicManager, Csr* pCsr, TrapProcessor* pTrapProcessor, IntRegFile* pIntRegFile, FpRegFile* pFpRegFile, MemoryAccessUnit* pMemAccessUnit)
        : m_pAtomicManager(pAtomicManager)
        , m_pCsr(pCsr)
        , m_pTrapProcessor(pTrapProcessor)
        , m_pIntRegFile(pIntRegFile)
        , m_pFpRegFile(pFpRegFile)
        , m_pMemAccessUnit(pMemAccessUnit)
    {
    }

    std::optional<Trap> PreCheckTrap(const Op& op, vaddr_t pc, uint32_t insn) const;

    std::optional<Trap> PostCheckTrap(const Op& op, vaddr_t pc) const;

    void ProcessOp(const Op& op, vaddr_t pc);

private:
    // PreCheckTrap
    std::optional<Trap> PreCheckTrapRV32_Load(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32_LoadReserved(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32_Store(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32_StoreConditional(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32_Atomic(const Op& op, vaddr_t pc) const;

    std::optional<Trap> PreCheckTrapRV64_Load(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64_LoadReserved(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64_Store(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64_StoreConditional(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64_Atomic(const Op& op, vaddr_t pc) const;

    std::optional<Trap> PreCheckTrap_Csr(const Op& op, vaddr_t pc, uint32_t insn) const;
    std::optional<Trap> PreCheckTrap_CsrImm(const Op& op, vaddr_t pc, uint32_t insn) const;
    std::optional<Trap> PreCheckTrap_Wfi(vaddr_t pc, uint32_t insn) const;
    std::optional<Trap> PreCheckTrap_Fence(vaddr_t pc, uint32_t insn) const;
    std::optional<Trap> PreCheckTrap_Priv(const Op& op, vaddr_t pc, uint32_t insn) const;

    std::optional<Trap> PreCheckTrapRV32C(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64C(const Op& op, vaddr_t pc) const;

    // RV32C
    std::optional<Trap> PreCheckTrapRV32C_FLD(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32C_FLDSP(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32C_FLW(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32C_FLWSP(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32C_FSD(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32C_FSDSP(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32C_FSW(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32C_FSWSP(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32C_LW(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32C_LWSP(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32C_SW(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV32C_SWSP(const Op& op, vaddr_t pc) const;

    // RV64C
    std::optional<Trap> PreCheckTrapRV64C_FLD(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64C_FLDSP(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64C_FSD(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64C_FSDSP(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64C_LD(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64C_LDSP(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64C_LW(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64C_LWSP(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64C_SD(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64C_SDSP(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64C_SW(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapRV64C_SWSP(const Op& op, vaddr_t pc) const;

    // PostCheckTrap
    std::optional<Trap> PostCheckTrapForEcall(vaddr_t pc) const;

    // Process
    void ProcessRV32I(const Op& op, vaddr_t pc);
    void ProcessRV32M(const Op& op);
    void ProcessRV32A(const Op& op);
    void ProcessRV32C(const Op& op, vaddr_t pc);

    void ProcessRV64I(const Op& op, vaddr_t pc);
    void ProcessRV64M(const Op& op);
    void ProcessRV64A(const Op& op);
    void ProcessRV64C(const Op& op, vaddr_t pc);

    void ProcessRVF(const Op& op);
    void ProcessRVD(const Op& op);

    // RV32I
    void ProcessRV32I_Lui(const Op& op);
    void ProcessRV32I_Auipc(const Op& op, vaddr_t pc);
    void ProcessRV32I_Jal(const Op& op, vaddr_t pc);
    void ProcessRV32I_Jalr(const Op& op, vaddr_t pc);
    void ProcessRV32I_Branch(const Op& op, vaddr_t pc);
    void ProcessRV32I_Load(const Op& op);
    void ProcessRV32I_Store(const Op& op);
    void ProcessRV32I_Alu(const Op& op);
    void ProcessRV32I_AluImm(const Op& op);
    void ProcessRV32I_Shift(const Op& op);
    void ProcessRV32I_ShiftImm(const Op& op);
    void ProcessRV32I_Fence();
    void ProcessRV32I_Priv(const Op& op);
    void ProcessRV32I_Csr(const Op& op);
    void ProcessRV32I_CsrImm(const Op& op);

    // RV64I
    void ProcessRV64I_Lui(const Op& op);
    void ProcessRV64I_Auipc(const Op& op, vaddr_t pc);
    void ProcessRV64I_Jal(const Op& op, vaddr_t pc);
    void ProcessRV64I_Jalr(const Op& op, vaddr_t pc);
    void ProcessRV64I_Branch(const Op& op, vaddr_t pc);
    void ProcessRV64I_Load(const Op& op);
    void ProcessRV64I_Store(const Op& op);
    void ProcessRV64I_Alu(const Op& op);
    void ProcessRV64I_AluImm(const Op& op);
    void ProcessRV64I_Shift(const Op& op);
    void ProcessRV64I_ShiftImm(const Op& op);
    void ProcessRV64I_Fence();
    void ProcessRV64I_Priv(const Op& op);
    void ProcessRV64I_Csr(const Op& op);
    void ProcessRV64I_CsrImm(const Op& op);

    // RV32C
    void ProcessRV32C_Alu(const Op& op);
    void ProcessRV32C_AluImm(const Op& op);
    void ProcessRV32C_Branch(const Op& op, vaddr_t pc);
    void ProcessRV32C_ADDI4SPN(const Op& op);
    void ProcessRV32C_ADDI16SP(const Op& op);
    void ProcessRV32C_FLD(const Op& op);
    void ProcessRV32C_FLDSP(const Op& op);
    void ProcessRV32C_FLW(const Op& op);
    void ProcessRV32C_FLWSP(const Op& op);
    void ProcessRV32C_FSD(const Op& op);
    void ProcessRV32C_FSDSP(const Op& op);
    void ProcessRV32C_FSW(const Op& op);
    void ProcessRV32C_FSWSP(const Op& op);
    void ProcessRV32C_J(const Op& op, vaddr_t pc);
    void ProcessRV32C_JAL(const Op& op, vaddr_t pc);
    void ProcessRV32C_JR(const Op& op);
    void ProcessRV32C_JALR(const Op& op, vaddr_t pc);
    void ProcessRV32C_LW(const Op& op);
    void ProcessRV32C_LWSP(const Op& op);
    void ProcessRV32C_SW(const Op& op);
    void ProcessRV32C_SWSP(const Op& op);

    // RV64C
    void ProcessRV64C_Alu(const Op& op);
    void ProcessRV64C_AluImm(const Op& op);
    void ProcessRV64C_Branch(const Op& op, vaddr_t pc);
    void ProcessRV64C_ADDI4SPN(const Op& op);
    void ProcessRV64C_ADDI16SP(const Op& op);
    void ProcessRV64C_FLD(const Op& op);
    void ProcessRV64C_FLDSP(const Op& op);
    void ProcessRV64C_FSD(const Op& op);
    void ProcessRV64C_FSDSP(const Op& op);
    void ProcessRV64C_J(const Op& op, vaddr_t pc);
    void ProcessRV64C_JR(const Op& op);
    void ProcessRV64C_JALR(const Op& op, vaddr_t pc);
    void ProcessRV64C_LD(const Op& op);
    void ProcessRV64C_LDSP(const Op& op);
    void ProcessRV64C_LW(const Op& op);
    void ProcessRV64C_LWSP(const Op& op);
    void ProcessRV64C_SD(const Op& op);
    void ProcessRV64C_SDSP(const Op& op);
    void ProcessRV64C_SW(const Op& op);
    void ProcessRV64C_SWSP(const Op& op);

    // RV32A / RV64A
    void ProcessRV32A_Atomic(const Op& op);
    void ProcessRV32A_Load(const Op& op);
    void ProcessRV32A_Store(const Op& op);

    void ProcessRV64A_Atomic32(const Op& op);
    void ProcessRV64A_Atomic64(const Op& op);
    void ProcessRV64A_Load32(const Op& op);
    void ProcessRV64A_Load64(const Op& op);
    void ProcessRV64A_Store32(const Op& op);
    void ProcessRV64A_Store64(const Op& op);

    // RV32F / RV64F
    void ProcessRVF_MulAdd(const Op& op);
    void ProcessRVF_Compute(const Op& op);
    void ProcessRVF_Compare(const Op& op);
    void ProcessRVF_Class(const Op& op);
    void ProcessRVF_MoveToInt(const Op& op);
    void ProcessRVF_MoveToFp(const Op& op);
    void ProcessRVF_ConvertToInt32(const Op& op);
    void ProcessRVF_ConvertToInt64(const Op& op);
    void ProcessRVF_ConvertSign(const Op& op);
    void ProcessRVF_Load(const Op& op);
    void ProcessRVF_Store(const Op& op);

    void ProcessRV32F_Load(const Op& op);
    void ProcessRV32F_Store(const Op& op);

    void ProcessRV64F_Load(const Op& op);
    void ProcessRV64F_Store(const Op& op);

    // RV32D / RV64D
    void ProcessRVD_MulAdd(const Op& op);
    void ProcessRVD_Compute(const Op& op);
    void ProcessRVD_Compare(const Op& op);
    void ProcessRVD_Class(const Op& op);
    void ProcessRVD_MoveToInt(const Op& op);
    void ProcessRVD_MoveToFp(const Op& op);
    void ProcessRVD_ConvertToInt32(const Op& op);
    void ProcessRVD_ConvertToInt64(const Op& op);
    void ProcessRVD_ConvertFp32ToFp64(const Op& op);
    void ProcessRVD_ConvertFp64ToFp32(const Op& op);
    void ProcessRVD_ConvertSign(const Op& op);

    void ProcessRV32D_Load(const Op& op);
    void ProcessRV32D_Store(const Op& op);

    void ProcessRV64D_Load(const Op& op);
    void ProcessRV64D_Store(const Op& op);

    // Common
    uint32_t ReadStackPointer32() const;
    uint64_t ReadStackPointer64() const;
    void WriteLinkRegister32(uint32_t value);
    void WriteLinkRegister64(uint64_t value);

    bool IsFpEnabled() const;
    void NotifyFpDirty();
    void UpdateFpCsr();

    [[noreturn]] void Error(const Op& op);

    AtomicManager* m_pAtomicManager;
    Csr* m_pCsr;
    TrapProcessor* m_pTrapProcessor;
    IntRegFile* m_pIntRegFile;
    FpRegFile* m_pFpRegFile;
    MemoryAccessUnit* m_pMemAccessUnit;
};

}}}
