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

#include <cstring>
#include <optional>

#include <rafi/emu.h>

#include "Trap.h"

namespace rafi { namespace emu { namespace cpu {

class Csr
{
public:
    explicit Csr(XLEN xlen, vaddr_t initialPc);

    std::optional<Trap> CheckTrap(csr_addr_t addr, bool write, vaddr_t pc, uint32_t insn) const;

    // Update registers for cycle
    void Update();

    // Special register access
    vaddr_t GetProgramCounter() const;
    void SetProgramCounter(vaddr_t value);

    PrivilegeLevel GetPrivilegeLevel() const;
    void SetPrivilegeLevel(PrivilegeLevel level);

    // Register access
    uint32_t ReadUInt32(csr_addr_t addr) const;
    uint64_t ReadUInt64(csr_addr_t addr) const;
    void WriteUInt32(csr_addr_t addr, uint32_t value);
    void WriteUInt64(csr_addr_t addr, uint64_t value);

    // Direct register access
    fcsr_t ReadFpCsr() const;
    xip_t ReadInterruptPending() const;
    xie_t ReadInterruptEnable() const;
    xstatus_t ReadStatus() const;
    satp_t ReadSatp() const;
    uint64_t ReadTime() const;

    void WriteFpCsr(const fcsr_t& value);
    void WriteInterruptPending(const xip_t& value);
    void WriteStatus(const xstatus_t& value);
    void WriteTime(uint64_t value);

    // for Dump
    int GetRegCount() const;
    void Copy(trace::Csr32Node* pOutNodes, int nodeCount) const;
    void Copy(trace::Csr64Node* pOutNodes, int nodeCount) const;

private:
    static const int RegisterAddrWidth = 12;
	static const int NumberOfRegister = 1 << RegisterAddrWidth;
	static const int NumberOfPerformanceCounter = 0x20;

    bool IsUserModeRegister(csr_addr_t addr) const;
    bool IsSupervisorModeRegister(csr_addr_t addr) const;
    bool IsReservedModeRegister(csr_addr_t addr) const;
    bool IsMachineModeRegister(csr_addr_t addr) const;

    uint64_t ReadMachineModeRegister(csr_addr_t addr) const;
	uint64_t ReadSupervisorModeRegister(csr_addr_t addr) const;
	uint64_t ReadUserModeRegister(csr_addr_t addr) const;

	void WriteMachineModeRegister(csr_addr_t addr, uint64_t value);
    void WriteSupervisorModeRegister(csr_addr_t addr, uint64_t value);
    void WriteUserModeRegister(csr_addr_t addr, uint64_t value);

    int GetPerformanceCounterIndex(csr_addr_t addr) const;
    void PrintRegisterUnimplementedMessage(csr_addr_t addr) const;

    // Configuration
    XLEN m_XLEN;
    misa_t m_ISA;

    // Floating point
    fcsr_t m_FpCsr {0};

    // Trap Setup (0x000-0x03f, 0x100-0x13f and 0x300-0x33f)
    xstatus_t m_Status {0};

    xtvec_t m_MachineTrapVector {0};
    xtvec_t m_SupervisorTrapVector {0};
    xtvec_t m_UserTrapVector {0};

    uint64_t m_MachineExceptionDelegation {0};
    uint64_t m_SupervisorExceptionDelegation {0};

    uint64_t m_MachineInterruptDelegation {0};
    uint64_t m_SupervisorInterruptDelegation {0};

    uint64_t m_MachineCounterEnable {0};
    uint64_t m_SupervisorCounterEnable {0};

    // Trap Handling (0x040-0x07f, 0x140-0x17f and 0x340-0x37f)
    uint64_t m_MachineScratch {0};
    uint64_t m_SupervisorScratch {0};
    uint64_t m_UserScratch {0};

    vaddr_t m_MachineExceptionProgramCounter {0};
    vaddr_t m_SupervisorExceptionProgramCounter {0};
    vaddr_t m_UserExceptionProgramCounter {0};

    uint64_t m_MachineCause {0};
    uint64_t m_SupervisorCause {0};
    uint64_t m_UserCause {0};

    uint64_t m_MachineTrapValue {0};
    uint64_t m_SupervisorTrapValue {0};
    uint64_t m_UserTrapValue {0};

    // Interrupt
    xie_t m_InterruptEnable {0};
    xip_t m_InterruptPending {0};

    // Protection and Translation (0x180-0x1bf and 0x380-0x3bf)
    satp_t m_SupervisorAddressTranslationProtection {0};

    // Performance Counters
    uint64_t m_CycleCounter {0};
    uint64_t m_TimeCounter {0};
    uint64_t m_InstructionRetiredCounter {0};

    // Special registers
    vaddr_t m_ProgramCounter {0};
    PrivilegeLevel m_PrivilegeLevel {PrivilegeLevel::Machine};
};

}}}
