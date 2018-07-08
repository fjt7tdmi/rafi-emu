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

#include "ProcessorException.h"
#include "Op.h"
#include "MemoryAccessUnit.h"
#include "RegisterFile.h"
#include "ControlStatusRegister.h"

#include "../../Common/Event.h"

class Processor
{
public:
    // Setup
    Processor(Bus* pBus, int32_t initialPc)
        : m_Csr(initialPc)
        , m_OpCount(0)
        , m_ReserveAddress(0)
    {
        std::memset(&m_OpEvent, 0, sizeof(m_OpEvent));

        m_Memory.Initialize(pBus, &m_Csr);
    }

    void SetIntReg(int regId, int32_t regValue);

    // Process
    void ProcessOneCycle();

    // for Dump
    int GetCsrSize() const;

    void CopyCsr(void* pOut, size_t size) const;
    void CopyIntRegs(void* pOut, size_t size) const;

    void CopyCsrReadEvent(CsrReadEvent* pOut) const;
    void CopyCsrWriteEvent(CsrWriteEvent* pOut) const;
    void CopyOpEvent(OpEvent* pOut) const;
    void CopyMemoryAccessEvent(MemoryAccessEvent* pOut) const;
    void CopyTrapEvent(TrapEvent* pOut) const;

    bool IsCsrReadEventExist() const;
    bool IsCsrWriteEventExist() const;
    bool IsMemoryAccessEventExist() const;
    bool IsOpEventExist() const;
    bool IsTrapEventExist() const;
private:
    // TODO: refactor and remove this def
    const int32_t InvalidValue = 0xcdcdcdcd;

    void PreCheckException(const Op& op, int32_t pc, int32_t insn);
    void PostCheckException(const Op& op, int32_t pc);

    void ProcessOp(const Op& op, int32_t pc);
    void ProcessRV32I(const Op& op, int32_t pc);
    void ProcessRV32M(const Op& op);
    void ProcessRV32A(const Op& op);

    ControlStatusRegister m_Csr;
    MemoryAccessUnit m_Memory;
    OpDecoder m_Decoder;
    RegisterFile m_IntReg;

    int32_t m_OpCount = 0;
    int32_t m_ReserveAddress = 0;

    // for dump
    bool m_OpEventValid = false;

    OpEvent m_OpEvent;
};
