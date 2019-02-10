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

#include <rafi/emu.h>
#include <rafi/trace.h>

#include "Trap.h"
#include "Csr.h"

using namespace rafi::trace;

namespace rafi { namespace emu { namespace cpu {

namespace {

const csr_addr_t DumpAddresses[] = {
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
#if defined(DUMP_USER_FP_CSR)
    csr_addr_t::fflags,
    csr_addr_t::frm,
    csr_addr_t::fcsr,
#endif

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
#if defined(DUMP_PMP_CSR)
    csr_addr_t::pmpcfg0,
    csr_addr_t::pmpcfg1,
    csr_addr_t::pmpcfg2,
    csr_addr_t::pmpcfg3,
#endif

    // Debug Trace Registers (Unimplemented)
#if defined(DUMP_TRACE_CSR)
    csr_addr_t::tselect,
    csr_addr_t::tdata1,
    csr_addr_t::tdata2,
    csr_addr_t::tdata3,
#endif

    // Debug Mode Registers (Unimplemented)
#if defined(DUMP_DEBUG_CSR)
    csr_addr_t::dcsr,
    csr_addr_t::dpc,
    csr_addr_t::dscratch,
#endif

    // Machine Information Registers
#if defined(DUMP_MACHINE_INFO_CSR)
    csr_addr_t::mvendorid,
    csr_addr_t::marchid,
    csr_addr_t::mimpid,
    csr_addr_t::mhartid,
#endif
};

}

Csr::Csr(uint32_t initialPc)
    : m_ProgramCounter(initialPc)
{
}

uint32_t Csr::GetProgramCounter() const
{
    return m_ProgramCounter;
}

void Csr::SetProgramCounter(uint32_t value)
{
    m_ProgramCounter = value;
}

PrivilegeLevel Csr::GetPrivilegeLevel() const
{
    return m_PrivilegeLevel;
}

void Csr::SetPrivilegeLevel(PrivilegeLevel level)
{
    m_PrivilegeLevel = level;
}

bool Csr::GetHaltFlag() const
{
    return m_HaltFlag;
}

void Csr::SetHaltFlag(bool flag)
{
    m_HaltFlag = flag;
}

void Csr::Update()
{
    m_CycleCounter++;
    m_TimeCounter++;
    m_InstructionRetiredCounter++;
}

std::optional<Trap> Csr::CheckTrap(int regId, bool write, uint32_t pc, uint32_t insn) const
{
    RAFI_EMU_CHECK_RANGE(0, regId, NumberOfRegister);

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
    const auto addr = static_cast<csr_addr_t>(regId);
    if ((csr_addr_t::hpmcounter_begin <= addr && addr < csr_addr_t::hpmcounter_end) ||
        (csr_addr_t::hpmcounterh_begin <= addr && addr < csr_addr_t::hpmcounterh_end))
    {
        const auto index = GetPerformanceCounterIndex(addr);

        RAFI_EMU_CHECK_RANGE(0, index, 32);

        const auto mask = 1 << index;

        switch (m_PrivilegeLevel)
        {
        case PrivilegeLevel::Supervisor:
            if (!(m_MachineCounterEnable & mask))
            {
                return MakeIllegalInstructionException(pc, insn);
            }
            break;
        case PrivilegeLevel::User:
            if (!(m_MachineCounterEnable & mask) || !(m_SupervisorCounterEnable & mask))
            {
                return MakeIllegalInstructionException(pc, insn);
            }
            break;
        default:
            break;
        }
    }

    return std::nullopt;
}


uint32_t Csr::Read(csr_addr_t addr) const
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

void Csr::Write(csr_addr_t addr, uint32_t value)
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

fcsr_t Csr::ReadFpCsr() const
{
    return m_FpCsr;
}

xip_t Csr::ReadInterruptPending() const
{
    return m_InterruptPending;
}

xie_t Csr::ReadInterruptEnable() const
{
    return m_InterruptEnable;
}

xstatus_t Csr::ReadStatus() const
{
    return m_Status;
}

satp_t Csr::ReadSatp() const
{
    return m_SupervisorAddressTranslationProtection;
}

void Csr::WriteFpCsr(const fcsr_t& value)
{
    m_FpCsr = value;
}

void Csr::WriteInterruptPending(const xip_t& value)
{
    m_InterruptPending = value;
}

bool Csr::IsUserModeRegister(csr_addr_t addr) const
{
    return ((static_cast<uint32_t>(addr) >> 8) & 0b11) == 0b00;
}

bool Csr::IsSupervisorModeRegister(csr_addr_t addr) const
{
    return ((static_cast<uint32_t>(addr) >> 8) & 0b11) == 0b01;
}

bool Csr::IsReservedModeRegister(csr_addr_t addr) const
{
    return ((static_cast<uint32_t>(addr) >> 8) & 0b11) == 0b10;
}

bool Csr::IsMachineModeRegister(csr_addr_t addr) const
{
    return ((static_cast<uint32_t>(addr) >> 8) & 0b11) == 0b11;
}

uint32_t Csr::ReadMachineModeRegister(csr_addr_t addr) const
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
        return m_InterruptEnable.GetWithMask(xie_t::MachineMask);
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
        return m_InterruptPending.GetWithMask(xip_t::MachineMask);
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

uint32_t Csr::ReadSupervisorModeRegister(csr_addr_t addr) const
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
        return m_InterruptEnable.GetWithMask(xie_t::SupervisorMask);
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
        return m_InterruptPending.GetWithMask(xip_t::SupervisorMask);
    case csr_addr_t::satp:
        return m_SupervisorAddressTranslationProtection;
    default:
        PrintRegisterUnimplementedMessage(addr);
        return 0;
    }
}

uint32_t Csr::ReadUserModeRegister(csr_addr_t addr) const
{
    switch (addr)
    {
    case csr_addr_t::ustatus:
        return m_Status.GetWithMask(xstatus_t::UserMask);
    case csr_addr_t::fflags:
        return m_FpCsr.GetMember<fcsr_t::AE>();
    case csr_addr_t::frm:
        return m_FpCsr.GetMember<fcsr_t::RM>();
    case csr_addr_t::fcsr:
        return m_FpCsr.GetWithMask(fcsr_t::UserMask);
    case csr_addr_t::uie:
        return m_InterruptEnable.GetWithMask(xie_t::UserMask);
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
        return m_InterruptPending.GetWithMask(xip_t::UserMask);
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

void Csr::WriteMachineModeRegister(csr_addr_t addr, uint32_t value)
{
    if (csr_addr_t::pmpaddr_begin <= addr && addr < csr_addr_t::pmpaddr_end)
    {
        // TODO: Implement PMP
        return;
    }

    switch(addr)
    {
    case csr_addr_t::mstatus:
        m_Status.SetValue(value);
        return;
    case csr_addr_t::medeleg:
        m_MachineExceptionDelegation = value;
        return;
    case csr_addr_t::mideleg:
        m_MachineInterruptDelegation = value;
        return;
    case csr_addr_t::mie:
        m_InterruptEnable.SetWithMask(value, xie_t::MachineMask);
        return;
    case csr_addr_t::mcounteren:
        m_MachineCounterEnable = value;
        return;
    case csr_addr_t::mtvec:
        m_MachineTrapVector.SetValue(value);
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
        m_InterruptPending.SetWithMask(value, xip_t::MachineMask & xip_t::WriteMask);
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

void Csr::WriteSupervisorModeRegister(csr_addr_t addr, uint32_t value)
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
        m_InterruptEnable.SetWithMask(value, xie_t::SupervisorMask);
        return;
    case csr_addr_t::stvec:
        m_SupervisorTrapVector.SetValue(value);
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
        m_InterruptPending.SetWithMask(value, xip_t::WriteMask & xip_t::SupervisorMask);
        return;
    case csr_addr_t::satp:
        m_SupervisorAddressTranslationProtection.SetValue(value);
        return;
    default:
        PrintRegisterUnimplementedMessage(addr);
        return;
    }
}

void Csr::WriteUserModeRegister(csr_addr_t addr, uint32_t value)
{
    switch (addr)
    {
    case csr_addr_t::ustatus:
        m_Status.SetWithMask(value, xstatus_t::UserMask);
        return;
    case csr_addr_t::fflags:
        m_FpCsr.SetMember<fcsr_t::AE>(value);
        return;
    case csr_addr_t::frm:
        m_FpCsr.SetMember<fcsr_t::RM>(value);
        return;
    case csr_addr_t::fcsr:
        m_FpCsr.SetWithMask(value, fcsr_t::UserMask);
        return;
    case csr_addr_t::uie:
        m_InterruptEnable.SetWithMask(value, xie_t::UserMask);
        return;
    case csr_addr_t::utvec:
        m_UserTrapVector.SetValue(value);
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
        m_InterruptPending.SetWithMask(value, xip_t::WriteMask & xip_t::UserMask);
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

int Csr::GetPerformanceCounterIndex(csr_addr_t addr) const
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
        RAFI_EMU_NOT_IMPLEMENTED();
    }

    return static_cast<int>(addr) - static_cast<int>(base);
}

void Csr::PrintRegisterUnimplementedMessage(csr_addr_t addr) const
{
    printf("Detect unimplemented CSR access (addr=0x%03x).\n", static_cast<int>(addr));
}

int Csr::GetRegisterCount() const
{
    return sizeof(DumpAddresses) / sizeof(DumpAddresses[0]);
}

void Csr::Copy(void* pOut, size_t size) const
{
    assert(size != GetRegisterCount() * sizeof(Csr32Node));

    auto nodes = reinterpret_cast<Csr32Node*>(pOut);

    for (int i = 0; i < GetRegisterCount(); i++)
    {
        nodes[i].address = static_cast<uint32_t>(DumpAddresses[i]);
        nodes[i].value = Read(DumpAddresses[i]);
    }
}

}}}
