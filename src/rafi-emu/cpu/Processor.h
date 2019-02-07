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
#include "Executor.h"
#include "FpRegFile.h"
#include "InterruptController.h"
#include "IntRegFile.h"
#include "MemoryAccessUnit.h"
#include "Trap.h"
#include "TrapProcessor.h"

#include <rvtrace/common.h>
#include <rafi/emu.h>

namespace rafi { namespace emu { namespace cpu {

class Processor
{
public:
    // Setup
    Processor(bus::Bus* pBus, int32_t initialPc)
        : m_Csr(initialPc)
        , m_CsrAccessor(&m_Csr)
        , m_InterruptController(&m_Csr)
        , m_TrapProcessor(&m_Csr)
        , m_Executor(&m_Csr, &m_CsrAccessor, &m_TrapProcessor, &m_IntRegFile, &m_FpRegFile, &m_MemAccessUnit)
    {
        m_MemAccessUnit.Initialize(pBus, &m_Csr);
    }

    void SetIntReg(int regId, int32_t regValue);

    // Interrupt source
    void RegisterExternalInterruptSource(IInterruptSource* pInterruptSource);
    void RegisterTimerInterruptSource(IInterruptSource* pInterruptSource);

    // Process
    void ProcessOneCycle();

    // for Dump
    int GetCsrCount() const;

    void CopyCsr(void* pOut, size_t size) const;
    void CopyIntReg(void* pOut, size_t size) const;
    void CopyFpReg(void* pOut, size_t size) const;

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

    void PrintStatus() const;
    
private:
    void ClearOpEvent();

    void SetOpEvent(int32_t virtualPc, PrivilegeLevel privilegeLevel);
    void SetOpEvent(int32_t virtualPc, PhysicalAddress physicalPc, int32_t insn, OpCode opCode, PrivilegeLevel privilegeLevel);

    // TODO: refactor and remove this def
    const int32_t InvalidValue = 0xcdcdcdcd;

    Csr m_Csr;
    CsrAccessor m_CsrAccessor;
    InterruptController m_InterruptController;
    TrapProcessor m_TrapProcessor;

    Decoder m_Decoder;
    FpRegFile m_FpRegFile;
    IntRegFile m_IntRegFile;
    MemoryAccessUnit m_MemAccessUnit;

    Executor m_Executor;

    int32_t m_OpCount { 0 };

    // for dump
    bool m_OpEventValid { false };

    OpEvent m_OpEvent;
};

}}}
