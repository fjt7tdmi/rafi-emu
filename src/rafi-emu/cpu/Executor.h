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

    // RV32I
    void ProcessLui(const Op& op);
    void ProcessAuipc(const Op& op, vaddr_t pc);
    void ProcessJal(const Op& op, vaddr_t pc);
    void ProcessJalr(const Op& op, vaddr_t pc);
    void ProcessBranch(const Op& op, vaddr_t pc);
    void ProcessLoad(const Op& op);
    void ProcessStore(const Op& op);
    void ProcessAlu(const Op& op);
    void ProcessAluImm(const Op& op);
    void ProcessShift(const Op& op);
    void ProcessShiftImm(const Op& op);
    void ProcessFence(const Op& op);
    void ProcessPriv(const Op& op);
    void ProcessCsr(const Op& op);
    void ProcessCsrImm(const Op& op);

    // RV32F
    void ProcessFloatLoad(const Op& op);
    void ProcessFloatStore(const Op& op);
    void ProcessFloatMulAdd(const Op& op);
    void ProcessFloatCompute(const Op& op);
    void ProcessFloatCompare(const Op& op);
    void ProcessFloatClass(const Op& op);
    void ProcessFloatMoveToInt(const Op& op);
    void ProcessFloatMoveToFp(const Op& op);
    void ProcessFloatConvertToInt(const Op& op);
    void ProcessFloatConvertToFp(const Op& op);

    // RV32D
    void ProcessDoubleLoad(const Op& op);
    void ProcessDoubleStore(const Op& op);
    void ProcessDoubleMulAdd(const Op& op);
    void ProcessDoubleCompute(const Op& op);
    void ProcessDoubleCompare(const Op& op);
    void ProcessDoubleClass(const Op& op);
    void ProcessDoubleConvertToInt(const Op& op);
    void ProcessDoubleConvertToFp32(const Op& op);
    void ProcessDoubleConvertToFp64(const Op& op);

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
