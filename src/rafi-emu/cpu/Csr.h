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

#include <rafi/BitField.h>
#include <rafi/Event.h>

#include "CsrTypes.h"
#include "Trap.h"

using namespace rvtrace;

namespace rafi { namespace cpu {

class Csr
{
public:
    explicit Csr(int32_t initialPc);

    std::optional<Trap> CheckTrap(int addr, bool write, int32_t pc, int32_t insn) const;

    // Update registers for cycle
    void Update();

    // Special register access
    PrivilegeLevel GetPrivilegeLevel() const;
    void SetPrivilegeLevel(PrivilegeLevel level);

    int32_t GetProgramCounter() const;
    void SetProgramCounter(int32_t value);

    // Register access
    int32_t Read(csr_addr_t addr) const;
    void Write(csr_addr_t addr, int32_t value);

    template <typename T>
    T ReadAs(csr_addr_t addr) const
    {
        return T(Read(addr));
    }

    // Direct register access
    xip_t ReadInterruptPending() const;
    xie_t ReadInterruptEnable() const;
    xstatus_t ReadStatus() const;
    satp_t ReadSatp() const;

    void WriteInterruptPending(const xip_t& value);

    // for Dump
    size_t GetRegisterFileSize() const;
    void CopyRegisterFile(void* pOut, size_t size) const;

private:
    static const int RegisterAddrWidth = 12;
	static const int NumberOfRegister = 1 << RegisterAddrWidth;
	static const int NumberOfPerformanceCounter = 0x20;

    bool IsUserModeRegister(csr_addr_t addr) const;
    bool IsSupervisorModeRegister(csr_addr_t addr) const;
    bool IsReservedModeRegister(csr_addr_t addr) const;
    bool IsMachineModeRegister(csr_addr_t addr) const;

    int32_t ReadMachineModeRegister(csr_addr_t addr) const;
	int32_t ReadSupervisorModeRegister(csr_addr_t addr) const;
	int32_t ReadUserModeRegister(csr_addr_t addr) const;

	void WriteMachineModeRegister(csr_addr_t addr, int32_t value);
    void WriteSupervisorModeRegister(csr_addr_t addr, int32_t value);
    void WriteUserModeRegister(csr_addr_t addr, int32_t value);

    int GetPerformanceCounterIndex(csr_addr_t addr) const;
    void PrintRegisterUnimplementedMessage(csr_addr_t addr) const;

    // Trap Setup (0x000-0x03f, 0x100-0x13f and 0x300-0x33f)
    xstatus_t m_Status {0};

    xtvec_t m_MachineTrapVector {0};
    xtvec_t m_SupervisorTrapVector {0};
    xtvec_t m_UserTrapVector {0};

    int32_t m_MachineExceptionDelegation {0};
    int32_t m_SupervisorExceptionDelegation {0};

    int32_t m_MachineInterruptDelegation {0};
    int32_t m_SupervisorInterruptDelegation {0};

    int32_t m_MachineCounterEnable {0};
    int32_t m_SupervisorCounterEnable {0};

    // Trap Handling (0x040-0x07f, 0x140-0x17f and 0x340-0x37f)
    int32_t m_MachineScratch {0};
    int32_t m_SupervisorScratch {0};
    int32_t m_UserScratch {0};

    int32_t m_MachineExceptionProgramCounter {0};
    int32_t m_SupervisorExceptionProgramCounter {0};
    int32_t m_UserExceptionProgramCounter {0};

    int32_t m_MachineCause {0};
    int32_t m_SupervisorCause {0};
    int32_t m_UserCause {0};

    int32_t m_MachineTrapValue {0};
    int32_t m_SupervisorTrapValue {0};
    int32_t m_UserTrapValue {0};

    // Interrupt
    xie_t m_InterruptEnable {0};
    xip_t m_InterruptPending {0};

    // Protection and Translation (0x180-0x1bf and 0x380-0x3bf)
    satp_t m_SupervisorAddressTranslationProtection {0};

    // Performance Counters
    int64_t m_CycleCounter {0};
    int64_t m_TimeCounter {0};
    int64_t m_InstructionRetiredCounter {0};

    // Special registers
    int32_t m_ProgramCounter {0};
    PrivilegeLevel m_PrivilegeLevel {PrivilegeLevel::Machine};
};

}}
