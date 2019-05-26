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
#include "Executor.h"
#include "FpRegFile.h"
#include "InterruptController.h"
#include "IntRegFile.h"
#include "MemoryAccessUnit.h"
#include "Trap.h"
#include "TrapProcessor.h"

#include <rafi/common.h>
#include <rafi/emu.h>

namespace rafi { namespace emu { namespace cpu {

class Processor
{
public:
    // Setup
    Processor(XLEN xlen, bus::Bus* pBus, vaddr_t initialPc);

    void SetIntReg(int regId, uint32_t regValue);

    // Interrupt source
    void RegisterExternalInterruptSource(IInterruptSource* pInterruptSource);
    void RegisterTimerInterruptSource(IInterruptSource* pInterruptSource);

    // for clint and plic
    xip_t ReadInterruptPending() const;
    void WriteInterruptPending(const xip_t& value);

    uint64_t ReadTime() const;
    void WriteTime(uint64_t value);

    // Process
    void ProcessCycle();

    // for Dump
    vaddr_t GetPc() const;
    int GetCsrCount() const;
    size_t GetMemoryAccessEventCount() const;

    void CopyIntReg(trace::IntReg32Node* pOut) const;
    void CopyIntReg(trace::IntReg64Node* pOut) const;
    void CopyCsr(trace::Csr32Node* pOutNodes, int nodeCount) const;
    void CopyCsr(trace::Csr64Node* pOutNodes, int nodeCount) const;
    void CopyFpReg(void* pOut, size_t size) const;
    void CopyOpEvent(OpEvent* pOut) const;
    void CopyTrapEvent(TrapEvent* pOut) const;
    void CopyMemoryAccessEvent(MemoryAccessEvent* pOut, int index) const;

    bool IsOpEventExist() const;
    bool IsTrapEventExist() const;

    void PrintStatus() const;

private:
    std::optional<Trap> CheckFetchTrap(vaddr_t pc);
    uint32_t Fetch(paddr_t* pOutPhysicalPc, vaddr_t pc);

    void ClearOpEvent();

    void SetOpEvent(vaddr_t virtualPc, PrivilegeLevel privilegeLevel);
    void SetOpEvent(vaddr_t virtualPc, paddr_t physicalPc, uint32_t insn, PrivilegeLevel privilegeLevel);

    const vaddr_t InvalidValue = 0xffffffffffffffff;

    AtomicManager m_AtomicManager;
    Csr m_Csr;
    InterruptController m_InterruptController;
    TrapProcessor m_TrapProcessor;

    Decoder m_Decoder;
    FpRegFile m_FpRegFile;
    IntRegFile m_IntRegFile;
    MemoryAccessUnit m_MemAccessUnit;

    Executor m_Executor;

    uint32_t m_OpCount { 0 };

    // for dump
    bool m_OpEventValid { false };

    OpEvent m_OpEvent;
};

}}}
