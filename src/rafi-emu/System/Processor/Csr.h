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

#include "../../Common/BitField.h"
#include "../../Common/Event.h"

#include "CsrTypes.h"
#include "ProcessorException.h"

using namespace rvtrace;

class Csr
{
public:
    explicit Csr(int32_t initialPc)
        : m_PrivilegeLevel(PrivilegeLevel::Machine)
        , m_ProgramCounter(initialPc)
	{
        std::memset(&m_ReadEvent, 0, sizeof(m_ReadEvent));
        std::memset(&m_WriteEvent, 0, sizeof(m_WriteEvent));
        std::memset(&m_TrapEvent, 0, sizeof(m_TrapEvent));
	}

    PrivilegeLevel GetPrivilegeLevel() const
    {
        return m_PrivilegeLevel;
    }

    void SetPrivilegeLevel(PrivilegeLevel level)
    {
        m_PrivilegeLevel = level;
    }

    int32_t GetProgramCounter() const
    {
        return m_ProgramCounter;
    }

    void SetProgramCounter(int32_t value)
    {
        m_ProgramCounter = value;
    }

    // Update registers for cycle
    void Update();

    int32_t Read(int addr);
    void Write(int addr, int32_t value);

    // Exception handling
    void CheckException(int addr, bool write, int32_t pc, int32_t insn);
    void ProcessException(ProcessorException e);
    void ProcessTrapReturn(PrivilegeLevel level);

    // Address translation
    bool IsAddresssTranslationEnabled() const;
    int32_t GetPhysicalPageNumber() const;
    bool GetSupervisorUserMemory() const;
    bool GetMakeExecutableReadable() const;

    // for Dump
    size_t GetRegisterFileSize() const;
    void ClearEvent();
    void CopyRegisterFile(void* pOut, size_t size) const;
    void CopyReadEvent(CsrReadEvent* pOut) const;
    void CopyWriteEvent(CsrWriteEvent* pOut) const;
    void CopyTrapEvent(TrapEvent* pOut) const;
    bool IsReadEventExist() const;
    bool IsWriteEventExist() const;
    bool IsTrapEventExist() const;

private:
    static const int RegisterAddrWidth = 12;
	static const int NumberOfRegister = 1 << RegisterAddrWidth;
	static const int NumberOfPerformanceCounter = 0x20;

    int32_t ReadInternal(csr_addr_t addr) const;
    void WriteInternal(csr_addr_t addr, int32_t value);

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
    xstatus_t m_Status;

    xtvec_t m_MachineTrapVector;
    xtvec_t m_SupervisorTrapVector;
    xtvec_t m_UserTrapVector;

    int32_t m_MachineExceptionDelegation = 0;
    int32_t m_SupervisorExceptionDelegation = 0;

    int32_t m_MachineInterruptDelegation = 0;
    int32_t m_SupervisorInterruptDelegation = 0;

    xie_t m_MachineInterruptEnable;
    xie_t m_SupervisorInterruptEnable;
    xie_t m_UserInterruptEnable;

    int32_t m_MachineCounterEnable = 0;
    int32_t m_SupervisorCounterEnable = 0;

    // Trap Handling (0x040-0x07f, 0x140-0x17f and 0x340-0x37f)
    int32_t m_MachineScratch = 0;
    int32_t m_SupervisorScratch = 0;
    int32_t m_UserScratch = 0;

    int32_t m_MachineExceptionProgramCounter = 0;
    int32_t m_SupervisorExceptionProgramCounter = 0;
    int32_t m_UserExceptionProgramCounter = 0;

    int32_t m_MachineCause = 0;
    int32_t m_SupervisorCause = 0;
    int32_t m_UserCause = 0;

    int32_t m_MachineTrapValue = 0;
    int32_t m_SupervisorTrapValue = 0;
    int32_t m_UserTrapValue = 0;

    xip_t m_MachineInterruptPending;
    xip_t m_SupervisorInterruptPending;
    xip_t m_UserInterruptPending;

    // Protection and Translation (0x180-0x1bf and 0x380-0x3bf)
    satp_t m_SupervisorAddressTranslationProtection;

    // Performance Counters
    int64_t m_CycleCounter = 0;
    int64_t m_TimeCounter = 0;
    int64_t m_InstructionRetiredCounter = 0;

    // Special registers
    int32_t m_ProgramCounter;
    PrivilegeLevel m_PrivilegeLevel;

    // Events
    bool m_ReadEventValid = false;
    bool m_WriteEventValid = false;
    bool m_TrapEventValid = false;

    CsrReadEvent m_ReadEvent;
    CsrWriteEvent m_WriteEvent;
    TrapEvent m_TrapEvent;
};
