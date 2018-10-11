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

#include "Csr.h"
#include "CsrAccessor.h"
#include "IntRegFile.h"
#include "MemoryAccessUnit.h"
#include "OpTypes.h"
#include "Trap.h"
#include "TrapProcessor.h"

class Executor
{
public:
    Executor(Csr* pCsr, CsrAccessor* pCsrAccessor, TrapProcessor* pTrapProcessor, IntRegFile* pIntRegFile, MemoryAccessUnit* pMemAccessUnit)
        : m_pCsr(pCsr)
        , m_pCsrAccessor(pCsrAccessor)
        , m_pTrapProcessor(pTrapProcessor)
        , m_pIntRegFile(pIntRegFile)
        , m_pMemAccessUnit(pMemAccessUnit)
    {
    }

    std::optional<Trap> PreCheckTrap(const Op& op, int32_t pc, int32_t insn) const;

    std::optional<Trap> PostCheckTrap(const Op& op, int32_t pc) const;

    void ProcessOp(const Op& op, int32_t pc);

private:
    std::optional<Trap> PreCheckTrapForLoad(int32_t pc, int32_t address) const;
    std::optional<Trap> PreCheckTrapForStore(int32_t pc, int32_t address) const;
    std::optional<Trap> PreCheckTrapForCsr(const Op& op, int32_t pc, int32_t insn) const;
    std::optional<Trap> PreCheckTrapForAtomic(int32_t pc, int32_t address) const;

    std::optional<Trap> PostCheckTrapForEcall(int32_t pc) const;

    void ProcessRV32I(const Op& op, int32_t pc);
    void ProcessRV32M(const Op& op);
    void ProcessRV32A(const Op& op);

    Csr* m_pCsr;
    CsrAccessor* m_pCsrAccessor;
    TrapProcessor* m_pTrapProcessor;
    IntRegFile* m_pIntRegFile;
    MemoryAccessUnit* m_pMemAccessUnit;

    int32_t m_ReserveAddress = 0;
};
