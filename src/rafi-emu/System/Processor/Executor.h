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
#include "IntRegFile.h"
#include "MemoryAccessUnit.h"
#include "OpTypes.h"

class Executor
{
public:
    Executor(Csr* pCsr, CsrAccessor* pCsrAccessor, IntRegFile* pIntRegFile, MemoryAccessUnit* pMemAccessUnit)
        : m_pCsr(pCsr)
        , m_pCsrAccessor(pCsrAccessor)
        , m_pIntRegFile(pIntRegFile)
        , m_pMemAccessUnit(pMemAccessUnit)
    {
    }

    void PreCheckException(const Op& op, int32_t pc, int32_t insn);
    void PostCheckException(const Op& op, int32_t pc);

    void ProcessOp(const Op& op, int32_t pc);
    void ProcessRV32I(const Op& op, int32_t pc);
    void ProcessRV32M(const Op& op);
    void ProcessRV32A(const Op& op);

private:
    Csr* m_pCsr;
    CsrAccessor* m_pCsrAccessor;
    IntRegFile* m_pIntRegFile;
    MemoryAccessUnit* m_pMemAccessUnit;

    int32_t m_ReserveAddress = 0;
};
