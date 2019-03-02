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
    Executor(Csr* pCsr, TrapProcessor* pTrapProcessor, IntRegFile* pIntRegFile, FpRegFile* pFpRegFile, MemoryAccessUnit* pMemAccessUnit)
        : m_pCsr(pCsr)
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
    std::optional<Trap> PreCheckTrapForLoad(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapForLoadReserved(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapForStore(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapForStoreConditional(const Op& op, vaddr_t pc) const;
    std::optional<Trap> PreCheckTrapForCsr(const Op& op, vaddr_t pc, uint32_t insn) const;
    std::optional<Trap> PreCheckTrapForCsrImm(const Op& op, vaddr_t pc, uint32_t insn) const;
    std::optional<Trap> PreCheckTrapForAtomic(const Op& op, vaddr_t pc) const;

    std::optional<Trap> PostCheckTrapForEcall(vaddr_t pc) const;

    void ProcessRV32I(const Op& op, vaddr_t pc);
    void ProcessRV32M(const Op& op);
    void ProcessRV32A(const Op& op);
    void ProcessRV32F(const Op& op);
    void ProcessRV32D(const Op& op);
    void ProcessRV32C(const Op& op, vaddr_t pc);

    void ProcessRV64I(const Op& op, vaddr_t pc);
    void ProcessRV64M(const Op& op);
    void ProcessRV64A(const Op& op);
    void ProcessRV64F(const Op& op);
    void ProcessRV64D(const Op& op);
    void ProcessRV64C(const Op& op, vaddr_t pc);

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
    void ProcessRV32I_Fence(const Op& op);
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
    void ProcessRV64I_Fence(const Op& op);
    void ProcessRV64I_Priv(const Op& op);
    void ProcessRV64I_Csr(const Op& op);
    void ProcessRV64I_CsrImm(const Op& op);

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

    void ProcessRV32F_Load(const Op& op);
    void ProcessRV32F_Store(const Op& op);

    void ProcessRV64F_Load(const Op& op);
    void ProcessRV64F_Store(const Op& op);

    // RV32D
    void ProcessRV32D_Load(const Op& op);
    void ProcessRV32D_Store(const Op& op);
    void ProcessRV32D_MulAdd(const Op& op);
    void ProcessRV32D_Compute(const Op& op);
    void ProcessRV32D_Compare(const Op& op);
    void ProcessRV32D_Class(const Op& op);
    void ProcessRV32D_ConvertToInt(const Op& op);
    void ProcessRV32D_ConvertToFp32(const Op& op);
    void ProcessRV32D_ConvertToFp64(const Op& op);

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

    // Common
    uint32_t ReadStackPointer();
    void WriteLinkRegister(uint32_t value);

    void UpdateFpCsr();
    [[noreturn]] void Error(const Op& op);

    Csr* m_pCsr;
    TrapProcessor* m_pTrapProcessor;
    IntRegFile* m_pIntRegFile;
    FpRegFile* m_pFpRegFile;
    MemoryAccessUnit* m_pMemAccessUnit;

    vaddr_t m_ReserveAddress = 0;
};

}}}
