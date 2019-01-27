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

#include "TrapProcessor.h"

using namespace rvtrace;

namespace rafi { namespace emu { namespace cpu {

void TrapProcessor::ProcessException(const Trap& trap)
{
    const auto exceptionCode = static_cast<int32_t>(trap.type);
    const auto delegMask = 1 << exceptionCode;

    PrivilegeLevel nextPrivilegeLevel = PrivilegeLevel::Machine;
    if ((m_pCsr->Read(csr_addr_t::medeleg) & delegMask) != 0)
    {
        nextPrivilegeLevel = PrivilegeLevel::Supervisor;
        if ((m_pCsr->Read(csr_addr_t::sedeleg) & delegMask) != 0)
        {
            nextPrivilegeLevel = PrivilegeLevel::User;
        }
    }

    ProcessTrapEnter(false, exceptionCode, trap.trapValue, trap.pc, nextPrivilegeLevel);
}

void TrapProcessor::ProcessInterrupt(InterruptType type, int32_t pc)
{
    PrivilegeLevel nextPrivilegeLevel;

    switch (type)
    {
    case InterruptType::MachineExternal:
    case InterruptType::MachineTimer:
    case InterruptType::MachineSoftware:
        nextPrivilegeLevel = PrivilegeLevel::Machine;
        break;
    case InterruptType::SupervisorExternal:
    case InterruptType::SupervisorTimer:
    case InterruptType::SupervisorSoftware:
        nextPrivilegeLevel = PrivilegeLevel::Supervisor;
        break;
    case InterruptType::UserExternal:
    case InterruptType::UserTimer:
    case InterruptType::UserSoftware:
        nextPrivilegeLevel = PrivilegeLevel::User;
        break;
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }

    ProcessTrapEnter(true, static_cast<int32_t>(type), 0, pc, nextPrivilegeLevel);
}

void TrapProcessor::ProcessTrapReturn(PrivilegeLevel level)
{
    xstatus_t status;
    int32_t pc;

    int32_t previousLevel;
    int32_t previousInterruptEnable;

    switch (level)
    {
    case PrivilegeLevel::Machine:
        status = m_pCsr->ReadAs<xstatus_t>(csr_addr_t::mstatus);
        pc = m_pCsr->Read(csr_addr_t::mepc);

        previousLevel = status.GetMember<xstatus_t::MPP>();
        previousInterruptEnable = status.GetMember<xstatus_t::MPIE>();

        status.SetMember<xstatus_t::MPP>(0);
        status.SetMember<xstatus_t::MIE>(previousInterruptEnable);

        m_pCsr->Write(csr_addr_t::mstatus, status);
        m_pCsr->SetProgramCounter(pc);
        break;
    case PrivilegeLevel::Supervisor:
        status = m_pCsr->ReadAs<xstatus_t>(csr_addr_t::sstatus);
        pc = m_pCsr->Read(csr_addr_t::sepc);

        previousLevel = status.GetMember<xstatus_t::SPP>();
        previousInterruptEnable = status.GetMember<xstatus_t::SPIE>();

        status.SetMember<xstatus_t::SPP>(0);
        status.SetMember<xstatus_t::SIE>(previousInterruptEnable);

        m_pCsr->Write(csr_addr_t::sstatus, status);
        m_pCsr->SetProgramCounter(pc);
        break;
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }

    const auto nextPrivilegeLevel = static_cast<PrivilegeLevel>(previousLevel);

    // for Dump
    m_TrapEventValid = true;
    m_TrapEvent.trapType = TrapType::Return;
    m_TrapEvent.from = m_pCsr->GetPrivilegeLevel();
    m_TrapEvent.to = nextPrivilegeLevel;

    m_pCsr->SetPrivilegeLevel(nextPrivilegeLevel);
}
void TrapProcessor::ClearEvent()
{
    m_TrapEventValid = false;

    std::memset(&m_TrapEvent, 0, sizeof(m_TrapEvent));
}

void TrapProcessor::CopyTrapEvent(TrapEvent* pOut) const
{
    std::memcpy(pOut, &m_TrapEvent, sizeof(*pOut));
}

bool TrapProcessor::IsTrapEventExist() const
{
    return m_TrapEventValid;
}

void TrapProcessor::ProcessTrapEnter(bool isInterrupt, int32_t exceptionCode, int32_t trapValue, int32_t pc, PrivilegeLevel nextPrivilegeLevel)
{
    const auto prevPrivilegeLevel = static_cast<int>(m_pCsr->GetPrivilegeLevel());

    m_pCsr->SetPrivilegeLevel(nextPrivilegeLevel);

    const int32_t cause = (isInterrupt ? 0x80000000 : 0) | exceptionCode;

    xtvec_t trapVector;
    xstatus_t status;

    switch (nextPrivilegeLevel)
    {
    case PrivilegeLevel::Machine:
        status = m_pCsr->Read(csr_addr_t::mstatus);

        status.SetMember<xstatus_t::MPIE>(status.GetMember<xstatus_t::MIE>());
        status.SetMember<xstatus_t::MIE>(0);
        status.SetMember<xstatus_t::MPP>(prevPrivilegeLevel);

        m_pCsr->Write(csr_addr_t::mstatus, status);
        m_pCsr->Write(csr_addr_t::mcause, cause);
        m_pCsr->Write(csr_addr_t::mepc, pc);
        m_pCsr->Write(csr_addr_t::mtval, trapValue);

        trapVector = m_pCsr->ReadAs<xtvec_t>(csr_addr_t::mtvec);
        break;
    case PrivilegeLevel::Supervisor:
        status = m_pCsr->Read(csr_addr_t::sstatus);

        status.SetMember<xstatus_t::SPIE>(status.GetMember<xstatus_t::SIE>());
        status.SetMember<xstatus_t::SIE>(0);
        status.SetMember<xstatus_t::SPP>(prevPrivilegeLevel);

        m_pCsr->Write(csr_addr_t::sstatus, status);
        m_pCsr->Write(csr_addr_t::scause, cause);
        m_pCsr->Write(csr_addr_t::sepc, pc);
        m_pCsr->Write(csr_addr_t::stval, trapValue);

        trapVector = m_pCsr->ReadAs<xtvec_t>(csr_addr_t::stvec);
        break;
    case PrivilegeLevel::User:
        status = m_pCsr->Read(csr_addr_t::ustatus);

        status.SetMember<xstatus_t::UPIE>(status.GetMember<xstatus_t::UIE>());
        status.SetMember<xstatus_t::UIE>(0);

        m_pCsr->Write(csr_addr_t::ustatus, status);
        m_pCsr->Write(csr_addr_t::ucause, cause);
        m_pCsr->Write(csr_addr_t::uepc, pc);
        m_pCsr->Write(csr_addr_t::utval, trapValue);

        trapVector = m_pCsr->ReadAs<xtvec_t>(csr_addr_t::utvec);
        break;
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }

    int32_t base = trapVector.GetWithMask(xtvec_t::BASE::Mask);
    int32_t mode = trapVector.GetMember<xtvec_t::MODE>();

    if (isInterrupt && mode == static_cast<int32_t>(xtvec_t::Mode::Vectored))
    {
        m_pCsr->SetProgramCounter(base + exceptionCode * 4);
    }
    else
    {
        m_pCsr->SetProgramCounter(base);
    }

    // for Dump
    m_TrapEventValid = true;
    m_TrapEvent.trapType = isInterrupt ? TrapType::Interrupt : TrapType::Exception;
    m_TrapEvent.from = m_pCsr->GetPrivilegeLevel();
    m_TrapEvent.to = nextPrivilegeLevel;
    m_TrapEvent.trapCause = exceptionCode;
    m_TrapEvent.trapValue = trapValue;
}

}}}
