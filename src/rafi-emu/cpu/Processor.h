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
#include "Decoder.h"
#include "Executor.h"
#include "InterruptController.h"
#include "IntRegFile.h"
#include "MemoryAccessUnit.h"
#include "Trap.h"
#include "TrapProcessor.h"

#include <emu/Event.h>

class Processor
{
public:
    // Setup
    Processor(Bus* pBus, int32_t initialPc)
        : m_Csr(initialPc)
        , m_CsrAccessor(&m_Csr)
        , m_InterruptController(&m_Csr)
        , m_TrapProcessor(&m_Csr)
        , m_Executor(&m_Csr, &m_CsrAccessor, &m_TrapProcessor, &m_IntRegFile, &m_MemAccessUnit)
    {
        std::memset(&m_OpEvent, 0, sizeof(m_OpEvent));

        m_MemAccessUnit.Initialize(pBus, &m_Csr);
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
    void ClearOpEvent();

    void SetOpEvent(int32_t virtualPc);
    void SetOpEvent(int32_t virtualPc, PhysicalAddress physicalPc, int32_t insn, OpCode opCode);

    // TODO: refactor and remove this def
    const int32_t InvalidValue = 0xcdcdcdcd;

    Csr m_Csr;
    CsrAccessor m_CsrAccessor;
    InterruptController m_InterruptController;
    TrapProcessor m_TrapProcessor;

    Decoder m_Decoder;
    IntRegFile m_IntRegFile;
    MemoryAccessUnit m_MemAccessUnit;

    Executor m_Executor;

    int32_t m_OpCount { 0 };

    // for dump
    bool m_OpEventValid { false };

    OpEvent m_OpEvent;
};
