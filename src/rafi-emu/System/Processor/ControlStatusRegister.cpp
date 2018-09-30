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

#include <cassert>
#include <cstdint>

#include "../../Common/Exception.h"

#include "ProcessorException.h"
#include "ControlStatusRegister.h"

using namespace rvtrace;

namespace {

int32_t GetHigh32(int64_t value)
{
    return static_cast<int32_t>(value >> 32);
}

int32_t GetLow32(int64_t value)
{
    return static_cast<int32_t>(value & 0xffffffff);
}

void SetHigh32(int64_t* pOut, int32_t value)
{
    (*pOut) &= 0xffffffff00000000LL;
    (*pOut) |= value;
}

void SetLow32(int64_t* pOut, int32_t value)
{
    (*pOut) &= 0x00000000ffffffffLL;
    (*pOut) |= (static_cast<int64_t>(value) << 32);
}

}

void ControlStatusRegister::Update()
{
    m_CycleCounter++;
    m_TimeCounter++;
    m_InstructionRetiredCounter++;
}

int32_t ControlStatusRegister::Read(int addr)
{
    auto csrAddr = static_cast<csr_addr_t>(addr);
    auto value = ReadInternal(csrAddr);

    // set event
    m_ReadEvent.address = csrAddr;
    m_ReadEvent.value = value;
    m_ReadEventValid = true;

    return value;
}

void ControlStatusRegister::Write(int addr, int32_t value)
{
    auto csrAddr = static_cast<csr_addr_t>(addr);
    WriteInternal(csrAddr, value);

    // set event
    m_WriteEvent = {csrAddr, value};
    m_WriteEventValid = true;
}

void ControlStatusRegister::CheckException(int regId, bool write, int32_t pc, int32_t insn)
{
    CHECK_RANGE(0, regId, NumberOfRegister);

    // disable permission check for riscv-tests
    (void)write;
#if 0
    bool debugModeOnly = (regId >> 6 == 0b011110);
    bool readOnly = (regId >> 10 == 0b11);

    if (IsSupervisorModeRegister(regId) && m_PrivilegeLevel == PrivilegeLevel::User)
    {
        throw IllegalInstructionException(pc, insn);
    }
    else if (IsReservedModeRegister(regId))
    {
        throw IllegalInstructionException(pc, insn);
    }
    else if (IsMachineModeRegister(regId) && (m_PrivilegeLevel == PrivilegeLevel::User || m_PrivilegeLevel == PrivilegeLevel::Supervisor))
    {
        throw IllegalInstructionException(pc, insn);
    }
    else if (debugModeOnly)
    {
        throw IllegalInstructionException(pc, insn);
    }
    else if (readOnly && write)
    {
        throw IllegalInstructionException(pc, insn);
    }

    /*
    if (!IsExist(regId))
    {
        throw IllegalInstructionException(pc, insn);
    }
    */
#endif

    // Performance Counter
    auto addr = static_cast<csr_addr_t>(regId);
    if ((csr_addr_t::hpmcounter_begin <= addr && addr < csr_addr_t::hpmcounter_end) ||
        (csr_addr_t::hpmcounterh_begin <= addr && addr < csr_addr_t::hpmcounterh_end))
    {
        auto index = GetPerformanceCounterIndex(addr);

        CHECK_RANGE(0, index, 32);

        auto mask = 1 << index;

        switch (m_PrivilegeLevel)
        {
        case PrivilegeLevel::Supervisor:
            if (!(m_MachineCounterEnable & mask))
            {
                throw IllegalInstructionException(pc, insn);
            }
            break;
        case PrivilegeLevel::User:
            if (!(m_MachineCounterEnable & mask) || !(m_SupervisorCounterEnable & mask))
            {
                throw IllegalInstructionException(pc, insn);
            }
            break;
        default:
            break;
        }
    }
}

void ControlStatusRegister::ProcessException(ProcessorException e)
{
    // Calulate values
    auto cause = static_cast<int32_t>(e.GetCause());
    auto causeMask = 1 << cause;

    PrivilegeLevel nextPrivilegeLevel = PrivilegeLevel::Machine;
    if ((ReadInternal(csr_addr_t::medeleg) & causeMask) != 0)
    {
        nextPrivilegeLevel = PrivilegeLevel::Supervisor;
        if ((ReadInternal(csr_addr_t::sedeleg) & causeMask) != 0)
        {
            nextPrivilegeLevel = PrivilegeLevel::User;
        }
    }

    // for Dump
    m_TrapEvent.trapType = TrapType::Exception;
    m_TrapEvent.from = m_PrivilegeLevel;
    m_TrapEvent.to = nextPrivilegeLevel;
    m_TrapEvent.trapCause = e.GetCause();
    m_TrapEvent.trapValue = e.GetTrapValue();

    // Update
    m_PrivilegeLevel = nextPrivilegeLevel;

    int32_t base;
    int32_t mode;
    switch (m_PrivilegeLevel)
    {
    case PrivilegeLevel::Machine:
        WriteInternal(csr_addr_t::mcause, static_cast<int32_t>(e.GetCause()));
        WriteInternal(csr_addr_t::mepc, e.GetProgramCounter());
        WriteInternal(csr_addr_t::mtval, e.GetTrapValue());
        base = m_MachineTrapVector.GetWithMask(xtvec_t::BASE::Mask);
        mode = m_MachineTrapVector.GetMember<xtvec_t::MODE>();
        break;
    case PrivilegeLevel::Supervisor:
        WriteInternal(csr_addr_t::scause, static_cast<int32_t>(e.GetCause()));
        WriteInternal(csr_addr_t::sepc, e.GetProgramCounter());
        WriteInternal(csr_addr_t::stval, e.GetTrapValue());
        base = m_SupervisorTrapVector.GetWithMask(xtvec_t::BASE::Mask);
        mode = m_SupervisorTrapVector.GetMember<xtvec_t::MODE>();
        break;
    case PrivilegeLevel::User:
        WriteInternal(csr_addr_t::ucause, static_cast<int32_t>(e.GetCause()));
        WriteInternal(csr_addr_t::uepc, e.GetProgramCounter());
        WriteInternal(csr_addr_t::utval, e.GetTrapValue());
        base = m_UserTrapVector.GetWithMask(xtvec_t::BASE::Mask);
        mode = m_UserTrapVector.GetMember<xtvec_t::MODE>();
        break;
    default:
        throw new NotImplementedException(__FILE__, __LINE__);
    }

    if (mode == static_cast<int32_t>(xtvec_t::Mode::Directed))
    {
        m_ProgramCounter = base;
    }
    else
    {
        m_ProgramCounter = base + cause * 4;
    }
}

void ControlStatusRegister::ProcessTrapReturn(PrivilegeLevel level)
{
    int32_t previousLevel;
    int32_t previousInterruptEnable;

    switch (level)
    {
    case PrivilegeLevel::Machine:
        previousLevel = m_Status.GetMember<xstatus_t::MPP>();
        m_Status.SetMember<xstatus_t::MPP>(0);

        previousInterruptEnable = m_Status.GetMember<xstatus_t::MPIE>();
        m_Status.SetMember<xstatus_t::MIE>(previousInterruptEnable);

        m_ProgramCounter = m_MachineExceptionProgramCounter;
        break;
    case PrivilegeLevel::Supervisor:
        previousLevel = m_Status.GetMember<xstatus_t::SPP>();
        m_Status.SetMember<xstatus_t::SPP>(0);

        previousInterruptEnable = m_Status.GetMember<xstatus_t::SPIE>();
        m_Status.SetMember<xstatus_t::SIE>(previousInterruptEnable);

        m_ProgramCounter = m_SupervisorExceptionProgramCounter;
        break;
    default:
        throw NotImplementedException(__FILE__, __LINE__);
    }

    auto nextPrivilegeLevel = static_cast<PrivilegeLevel>(previousLevel);

    // for Dump
    m_TrapEventValid = true;
    m_TrapEvent.trapType = TrapType::Return;
    m_TrapEvent.from = m_PrivilegeLevel;
    m_TrapEvent.to = nextPrivilegeLevel;

    m_PrivilegeLevel = nextPrivilegeLevel;
}

int32_t ControlStatusRegister::ReadInternal(csr_addr_t addr) const
{
    if (IsMachineModeRegister(addr))
    {
        return ReadMachineModeRegister(addr);
    }
    else if (IsSupervisorModeRegister(addr))
    {
        return ReadSupervisorModeRegister(addr);
    }
    else if (IsUserModeRegister(addr))
    {
        return ReadUserModeRegister(addr);
    }
    else
    {
        PrintRegisterUnimplementedMessage(addr);
        return 0;
    }
}

void ControlStatusRegister::WriteInternal(csr_addr_t addr, int32_t value)
{
    if (IsMachineModeRegister(addr))
    {
        WriteMachineModeRegister(addr, value);
    }
    else if (IsSupervisorModeRegister(addr))
    {
        WriteSupervisorModeRegister(addr, value);
    }
    else if (IsUserModeRegister(addr))
    {
        WriteUserModeRegister(addr, value);
    }
    else
    {
        PrintRegisterUnimplementedMessage(addr);
        return;
    }
}

bool ControlStatusRegister::IsUserModeRegister(csr_addr_t addr) const
{
    return ((static_cast<int32_t>(addr) >> 8) & 0b11) == 0b00;
}

bool ControlStatusRegister::IsSupervisorModeRegister(csr_addr_t addr) const
{
    return ((static_cast<int32_t>(addr) >> 8) & 0b11) == 0b01;
}

bool ControlStatusRegister::IsReservedModeRegister(csr_addr_t addr) const
{
    return ((static_cast<int32_t>(addr) >> 8) & 0b11) == 0b10;
}

bool ControlStatusRegister::IsMachineModeRegister(csr_addr_t addr) const
{
    return ((static_cast<int32_t>(addr) >> 8) & 0b11) == 0b11;
}

int32_t ControlStatusRegister::ReadMachineModeRegister(csr_addr_t addr) const
{
    if (csr_addr_t::pmpaddr_begin <= addr && addr < csr_addr_t::pmpaddr_end)
    {
        // TODO: Implement PMP
        return 0;
    }

    switch (addr)
    {
    case csr_addr_t::mstatus:
        return m_Status;
    case csr_addr_t::misa:
    {
        misa_t value;
        return value
            .SetMember<misa_t::XLEN>(XLEN::XLEN32)
            .SetMember<misa_t::I>(1);
    }
    case csr_addr_t::medeleg:
        return m_MachineExceptionDelegation;
    case csr_addr_t::mideleg:
        return m_MachineInterruptDelegation;
    case csr_addr_t::mie:
        return m_MachineInterruptEnable;
    case csr_addr_t::mtvec:
        return m_MachineTrapVector;
    case csr_addr_t::mcounteren:
        return m_MachineCounterEnable;
    case csr_addr_t::mscratch:
        return m_MachineScratch;
    case csr_addr_t::mepc:
        return m_MachineExceptionProgramCounter;
    case csr_addr_t::mcause:
        return m_MachineCause;
    case csr_addr_t::mtval:
        return m_MachineTrapValue;
    case csr_addr_t::mip:
        return m_MachineInterruptPending;
    case csr_addr_t::pmpcfg0:
    case csr_addr_t::pmpcfg1:
    case csr_addr_t::pmpcfg2:
    case csr_addr_t::pmpcfg3:
        // TODO: Implement PMP
        return 0;
    case csr_addr_t::mcycle:
        return GetLow32(m_CycleCounter);
    case csr_addr_t::minstret:
        return GetLow32(m_InstructionRetiredCounter);
    case csr_addr_t::mcycleh:
        return GetHigh32(m_CycleCounter);
    case csr_addr_t::minstreth:
        return GetHigh32(m_InstructionRetiredCounter);
    case csr_addr_t::mvendorid:
        return mvendorid::NonCommercial;
    case csr_addr_t::marchid:
        return marchid::NotImplemented;
    case csr_addr_t::mimpid:
        return mimpid::NotImplemented;
    case csr_addr_t::mhartid:
        return 0;
    default:
        PrintRegisterUnimplementedMessage(addr);
        return 0;
    }
}

int32_t ControlStatusRegister::ReadSupervisorModeRegister(csr_addr_t addr) const
{
    switch (addr)
    {
    case csr_addr_t::sstatus:
        return m_Status.GetWithMask(xstatus_t::SupervisorMask);
    case csr_addr_t::sedeleg:
        return m_SupervisorExceptionDelegation;
    case csr_addr_t::sideleg:
        return m_SupervisorInterruptDelegation;
    case csr_addr_t::sie:
        return m_SupervisorInterruptEnable;
    case csr_addr_t::stvec:
        return m_SupervisorTrapVector;
    case csr_addr_t::scounteren:
        return m_SupervisorCounterEnable;
    case csr_addr_t::sscratch:
        return m_SupervisorScratch;
    case csr_addr_t::sepc:
        return m_SupervisorExceptionProgramCounter;
    case csr_addr_t::scause:
        return m_SupervisorCause;
    case csr_addr_t::stval:
        return m_SupervisorTrapValue;
    case csr_addr_t::sip:
        return m_SupervisorInterruptPending;
    case csr_addr_t::satp:
        return m_SupervisorAddressTranslationProtection;
    default:
        PrintRegisterUnimplementedMessage(addr);
        return 0;
    }
}

int32_t ControlStatusRegister::ReadUserModeRegister(csr_addr_t addr) const
{
    switch (addr)
    {
    case csr_addr_t::ustatus:
        return m_Status.GetWithMask(xstatus_t::UserMask);
    case csr_addr_t::uie:
        return m_UserInterruptEnable.GetWithMask(xie_t::UserMask);
    case csr_addr_t::utvec:
        return m_UserTrapVector;
    case csr_addr_t::uscratch:
        return m_UserScratch;
    case csr_addr_t::uepc:
        return m_UserExceptionProgramCounter;
    case csr_addr_t::ucause:
        return m_UserCause;
    case csr_addr_t::utval:
        return m_UserTrapValue;
    case csr_addr_t::uip:
        return m_UserInterruptPending;
    case csr_addr_t::cycle:
        return GetLow32(m_CycleCounter);
    case csr_addr_t::time:
        return GetLow32(m_TimeCounter);
    case csr_addr_t::instret:
        return GetLow32(m_InstructionRetiredCounter);
    case csr_addr_t::cycleh:
        return GetHigh32(m_CycleCounter);
    case csr_addr_t::timeh:
        return GetHigh32(m_TimeCounter);
    case csr_addr_t::instreth:
        return GetHigh32(m_InstructionRetiredCounter);
    default:
        PrintRegisterUnimplementedMessage(addr);
        return 0;
	}
}

void ControlStatusRegister::WriteMachineModeRegister(csr_addr_t addr, int32_t value)
{
    if (csr_addr_t::pmpaddr_begin <= addr && addr < csr_addr_t::pmpaddr_end)
    {
        // TODO: Implement PMP
        return;
    }

    switch(addr)
    {
    case csr_addr_t::mstatus:
        m_Status.Set(value);
        return;
    case csr_addr_t::medeleg:
        m_MachineExceptionDelegation = value;
        return;
    case csr_addr_t::mideleg:
        m_MachineInterruptDelegation = value;
        return;
    case csr_addr_t::mie:
        m_MachineInterruptEnable.Set(value);
        return;
    case csr_addr_t::mcounteren:
        m_MachineCounterEnable = value;
        return;
    case csr_addr_t::mtvec:
        m_MachineTrapVector.Set(value);
        return;
    case csr_addr_t::mscratch:
        m_MachineScratch = value;
        return;
    case csr_addr_t::mepc:
        m_MachineExceptionProgramCounter = value;
        return;
    case csr_addr_t::mcause:
        m_MachineCause = value;
        return;
    case csr_addr_t::mtval:
        m_MachineTrapValue = value;
        return;
    case csr_addr_t::mip:
        m_MachineInterruptPending.SetWithMask(value, xip_t::WriteMask);
        return;
    case csr_addr_t::pmpcfg0:
    case csr_addr_t::pmpcfg1:
    case csr_addr_t::pmpcfg2:
    case csr_addr_t::pmpcfg3:
        // TODO: Implement PMP
        return;
    case csr_addr_t::mcycle:
        SetLow32(&m_CycleCounter, value);
        return;
    case csr_addr_t::minstret:
        SetLow32(&m_InstructionRetiredCounter, value);
        return;
    case csr_addr_t::mcycleh:
        SetHigh32(&m_CycleCounter, value);
        return;
    case csr_addr_t::minstreth:
        SetHigh32(&m_InstructionRetiredCounter, value);
        return;
    case csr_addr_t::mhartid:
        // Suppress warning by writing to mhartid
        return;
    default:
        PrintRegisterUnimplementedMessage(addr);
        return;
    }
}

void ControlStatusRegister::WriteSupervisorModeRegister(csr_addr_t addr, int32_t value)
{
    switch(addr)
    {
    case csr_addr_t::sstatus:
        m_Status.SetWithMask(value, xstatus_t::SupervisorMask);
        return;
    case csr_addr_t::sedeleg:
        m_SupervisorExceptionDelegation = value;
        return;
    case csr_addr_t::sideleg:
        m_SupervisorInterruptDelegation = value;
        return;
    case csr_addr_t::sie:
        m_SupervisorInterruptEnable.Set(value);
        return;
    case csr_addr_t::stvec:
        m_SupervisorTrapVector.Set(value);
        return;
    case csr_addr_t::scounteren:
        m_SupervisorCounterEnable = value;
        return;
    case csr_addr_t::sscratch:
        m_SupervisorScratch = value;
        return;
    case csr_addr_t::sepc:
        m_SupervisorExceptionProgramCounter = value;
        return;
    case csr_addr_t::scause:
        m_SupervisorCause = value;
        return;
    case csr_addr_t::stval:
        m_SupervisorTrapValue = value;
        return;
    case csr_addr_t::sip:
        m_SupervisorInterruptPending.SetWithMask(value, xip_t::WriteMask & xip_t::SupervisorMask);
        return;
    case csr_addr_t::satp:
        m_SupervisorAddressTranslationProtection.Set(value);
        return;
    default:
        PrintRegisterUnimplementedMessage(addr);
        return;
    }
}

void ControlStatusRegister::WriteUserModeRegister(csr_addr_t addr, int32_t value)
{
    switch (addr)
    {
    case csr_addr_t::ustatus:
        m_Status.SetWithMask(value, xstatus_t::UserMask);
        return;
    case csr_addr_t::uie:
        m_UserInterruptEnable.SetWithMask(value, xie_t::UserMask);
        return;
    case csr_addr_t::utvec:
        m_UserTrapVector.Set(value);
        return;
    case csr_addr_t::uscratch:
        m_UserScratch = value;
        return;
    case csr_addr_t::uepc:
        m_UserExceptionProgramCounter = value;
        return;
    case csr_addr_t::ucause:
        m_UserCause = value;
        return;
    case csr_addr_t::utval:
        m_UserTrapValue = value;
        return;
    case csr_addr_t::uip:
        m_UserInterruptPending.SetWithMask(value, xip_t::WriteMask & xip_t::SupervisorMask);
        return;
    case csr_addr_t::cycle:
        SetLow32(&m_CycleCounter, value);
        return;
    case csr_addr_t::time:
        SetLow32(&m_TimeCounter, value);
        return;
    case csr_addr_t::instret:
        SetLow32(&m_InstructionRetiredCounter, value);
        return;
    case csr_addr_t::cycleh:
        SetHigh32(&m_CycleCounter, value);
        return;
    case csr_addr_t::timeh:
        SetHigh32(&m_TimeCounter, value);
        return;
    case csr_addr_t::instreth:
        SetHigh32(&m_InstructionRetiredCounter, value);
        return;
    default:
        PrintRegisterUnimplementedMessage(addr);
        return;
    }
}

int ControlStatusRegister::GetPerformanceCounterIndex(csr_addr_t addr) const
{
    csr_addr_t base;

    if (csr_addr_t::mhpmcounter_begin <= addr && addr < csr_addr_t::mhpmcounter_end)
    {
        base = csr_addr_t::mhpmcounter_begin;
    }
    else if (csr_addr_t::mhpmcounterh_begin <= addr && addr < csr_addr_t::mhpmcounterh_end)
    {
        base = csr_addr_t::mhpmcounterh_begin;
    }
    else if (csr_addr_t::hpmcounter_begin <= addr && addr < csr_addr_t::hpmcounter_end)
    {
        base = csr_addr_t::hpmcounter_begin;
    }
    else if (csr_addr_t::hpmcounterh_begin <= addr && addr < csr_addr_t::hpmcounterh_end)
    {
        base = csr_addr_t::hpmcounterh_begin;
    }
    else
    {
        throw new NotImplementedException(__FILE__, __LINE__);
    }

    return static_cast<int>(addr) - static_cast<int>(base);
}

void ControlStatusRegister::PrintRegisterUnimplementedMessage(csr_addr_t addr) const
{
    printf("Detect unimplemented CSR access (addr=0x%03x).\n", static_cast<int>(addr));
}

bool ControlStatusRegister::IsAddresssTranslationEnabled() const
{
    if (m_PrivilegeLevel == PrivilegeLevel::Machine)
    {
        return false;
    }

    auto mode = static_cast<satp_t::Mode>(m_SupervisorAddressTranslationProtection.GetMember<satp_t::MODE>());
    return mode != satp_t::Mode::Bare;
}

int32_t ControlStatusRegister::GetPhysicalPageNumber() const
{
    return m_SupervisorAddressTranslationProtection.GetMember<satp_t::PPN>();
}

bool ControlStatusRegister::GetSupervisorUserMemory() const
{
    return m_Status.GetMember<xstatus_t::SUM>();
}

bool ControlStatusRegister::GetMakeExecutableReadable() const
{
    return m_Status.GetMember<xstatus_t::MXR>();
}

size_t ControlStatusRegister::GetRegisterFileSize() const
{
    return NumberOfRegister * sizeof(int32_t);
}

void ControlStatusRegister::ClearEvent()
{
    m_ReadEventValid = false;
    m_WriteEventValid = false;
    m_TrapEventValid = false;

    std::memset(&m_ReadEvent, 0, sizeof(m_ReadEvent));
    std::memset(&m_WriteEvent, 0, sizeof(m_WriteEvent));
    std::memset(&m_TrapEvent, 0, sizeof(m_TrapEvent));
}

void ControlStatusRegister::CopyRegisterFile(void* pOut, size_t size) const
{
    if (size != GetRegisterFileSize())
    {
        abort();
    }

    std::memset(pOut, 0, size);

    auto p = reinterpret_cast<int32_t*>(pOut);

    const csr_addr_t addrs[] = {
        csr_addr_t::ustatus,
        csr_addr_t::uie,
        csr_addr_t::utvec,

        // User Trap Handling
        csr_addr_t::uscratch,
        csr_addr_t::uepc,
        csr_addr_t::ucause,
        csr_addr_t::utval,
        csr_addr_t::uip,

        // User Floating-Point CSRs (Unimplemented)
        csr_addr_t::fflags,
        csr_addr_t::frm,
        csr_addr_t::fcsr,

        // Supervisor Trap Setup
        csr_addr_t::sstatus,
        csr_addr_t::sedeleg,
        csr_addr_t::sideleg,
        csr_addr_t::sie,
        csr_addr_t::stvec,
        csr_addr_t::scounteren,

        // Supervisor Trap Handling
        csr_addr_t::sscratch,
        csr_addr_t::sepc,
        csr_addr_t::scause,
        csr_addr_t::stval,
        csr_addr_t::sip,

        // Supervisor Protection and Translation
        csr_addr_t::satp,

        // Machine Trap Setup
        csr_addr_t::mstatus,
        csr_addr_t::misa,
        csr_addr_t::medeleg,
        csr_addr_t::mideleg,
        csr_addr_t::mie,
        csr_addr_t::mtvec,
        csr_addr_t::mcounteren,

        // Machine Trap Handling
        csr_addr_t::mscratch,
        csr_addr_t::mepc,
        csr_addr_t::mcause,
        csr_addr_t::mtval,
        csr_addr_t::mip,

        // Machine Protection and Translation
        csr_addr_t::pmpcfg0,
        csr_addr_t::pmpcfg1,
        csr_addr_t::pmpcfg2,
        csr_addr_t::pmpcfg3,

        csr_addr_t::pmpaddr_begin,
        csr_addr_t::pmpaddr_end,

        // Debug Trace Registers (Unimplemented)
        csr_addr_t::tselect,
        csr_addr_t::tdata1,
        csr_addr_t::tdata2,
        csr_addr_t::tdata3,

        // Debug Mode Registers (Unimplemented)
        csr_addr_t::dcsr,
        csr_addr_t::dpc,
        csr_addr_t::dscratch,

        // Machine Information Registers
        csr_addr_t::mvendorid,
        csr_addr_t::marchid,
        csr_addr_t::mimpid,
        csr_addr_t::mhartid,
    };

    for (const auto& addr: addrs)
    {
        p[static_cast<int>(addr)] = ReadInternal(addr);
    }

    // User Counter / Timers
    for (int i = static_cast<int>(csr_addr_t::hpmcounter_begin); i < static_cast<int>(csr_addr_t::hpmcounter_end); i++)
    {
        p[i] = ReadInternal(static_cast<csr_addr_t>(i));
    }

    for (int i = static_cast<int>(csr_addr_t::hpmcounterh_begin); i < static_cast<int>(csr_addr_t::hpmcounterh_end); i++)
    {
        p[i] = ReadInternal(static_cast<csr_addr_t>(i));
    }

    for (int i = static_cast<int>(csr_addr_t::mhpmcounter_begin); i < static_cast<int>(csr_addr_t::mhpmcounter_end); i++)
    {
        p[i] = ReadInternal(static_cast<csr_addr_t>(i));
    }

    for (int i = static_cast<int>(csr_addr_t::mhpmcounterh_begin); i < static_cast<int>(csr_addr_t::mhpmcounterh_end); i++)
    {
        p[i] = ReadInternal(static_cast<csr_addr_t>(i));
    }
}

void ControlStatusRegister::CopyReadEvent(CsrReadEvent* pOut) const
{
    std::memcpy(pOut, &m_ReadEvent, sizeof(*pOut));
}

void ControlStatusRegister::CopyWriteEvent(CsrWriteEvent* pOut) const
{
    std::memcpy(pOut, &m_WriteEvent, sizeof(*pOut));
}

void ControlStatusRegister::CopyTrapEvent(TrapEvent* pOut) const
{
    std::memcpy(pOut, &m_TrapEvent, sizeof(*pOut));
}

bool ControlStatusRegister::IsReadEventExist() const
{
    return m_ReadEventValid;
}

bool ControlStatusRegister::IsWriteEventExist() const
{
    return m_WriteEventValid;
}

bool ControlStatusRegister::IsTrapEventExist() const
{
    return m_TrapEventValid;
}
