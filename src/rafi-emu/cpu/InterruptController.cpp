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
#include <cstring>

#include <rafi/emu.h>
#include "InterruptController.h"

namespace rafi { namespace emu { namespace cpu {

namespace {
    int32_t CountBits(int32_t value)
    {
        auto tmp = value;

        tmp = (tmp & 0x55555555) + (tmp >> 1 & 0x55555555);
        tmp = (tmp & 0x33333333) + (tmp >> 2 & 0x33333333);
        tmp = (tmp & 0x0f0f0f0f) + (tmp >> 4 & 0x0f0f0f0f);
        tmp = (tmp & 0x00ff00ff) + (tmp >> 8 & 0x00ff00ff);
        tmp = (tmp & 0x0000ffff) + (tmp >> 16 & 0x0000ffff);

        return tmp;
    }

    int32_t NumberOfTrainingZero(int32_t value)
    {
        return CountBits((value & (-value)) - 1);
    }
}

InterruptController::InterruptController(Csr* pCsr)
    : m_pCsr(pCsr)
{
}

InterruptType InterruptController::GetInterruptType() const
{
    assert(m_IsRequested);

    return m_InterruptType;
}

bool InterruptController::IsRequested() const
{
    return m_IsRequested;
}

void InterruptController::Update()
{
    UpdateCsr();

    const xstatus_t status = m_pCsr->ReadStatus();
    const xie_t ie = m_pCsr->ReadInterruptEnable();
    const xip_t ip = m_pCsr->ReadInterruptPending();

    m_IsRequested = false;

    // Check xIE
    switch (m_pCsr->GetPrivilegeLevel())
    {
    case PrivilegeLevel::Machine:
        if (!status.GetMember<xstatus_t::MIE>())
        {
            return;
        }
        break;
    case PrivilegeLevel::Supervisor:
        if (!status.GetMember<xstatus_t::SIE>())
        {
            return;
        }
        break;
    case PrivilegeLevel::User:
        if (!status.GetMember<xstatus_t::UIE>())
        {
            return;
        }
        break;
    default:
        RAFI_EMU_NOT_IMPLEMENTED;
    }

    // Check interrupt to M-mode
    if (ie.GetMember<xie_t::MEIE>() && ip.GetMember<xip_t::MEIP>())
    {
        m_IsRequested = true;
        m_InterruptType = InterruptType::MachineExternal;
        return;
    }
    if (ie.GetMember<xie_t::MTIE>() && ip.GetMember<xip_t::MTIP>())
    {
        m_IsRequested = true;
        m_InterruptType = InterruptType::MachineTimer;
        return;
    }
    if (ie.GetMember<xie_t::MSIE>() && ip.GetMember<xip_t::MSIP>())
    {
        m_IsRequested = true;
        m_InterruptType = InterruptType::MachineSoftware;
        return;
    }

    // Check interrupt to S-mode
    if (ie.GetMember<xie_t::SEIE>() && ip.GetMember<xip_t::SEIP>())
    {
        m_IsRequested = true;
        m_InterruptType = InterruptType::SupervisorExternal;
        return;
    }
    if (ie.GetMember<xie_t::STIE>() && ip.GetMember<xip_t::STIP>())
    {
        m_IsRequested = true;
        m_InterruptType = InterruptType::SupervisorTimer;
        return;
    }
    if (ie.GetMember<xie_t::SSIE>() && ip.GetMember<xip_t::SSIP>())
    {
        m_IsRequested = true;
        m_InterruptType = InterruptType::SupervisorSoftware;
        return;
    }

    // Check interrupt to U-mode
    if (ie.GetMember<xie_t::UEIE>() && ip.GetMember<xip_t::UEIP>())
    {
        m_IsRequested = true;
        m_InterruptType = InterruptType::UserExternal;
        return;
    }
    if (ie.GetMember<xie_t::UTIE>() && ip.GetMember<xip_t::UTIP>())
    {
        m_IsRequested = true;
        m_InterruptType = InterruptType::UserTimer;
        return;
    }
    if (ie.GetMember<xie_t::USIE>() && ip.GetMember<xip_t::USIP>())
    {
        m_IsRequested = true;
        m_InterruptType = InterruptType::UserSoftware;
        return;
    }
}

void InterruptController::RegisterExternalInterruptSource(IInterruptSource* pInterruptSource)
{
    assert(m_pExternalInterruptSource == nullptr);

    m_pExternalInterruptSource = pInterruptSource;
}

void InterruptController::RegisterTimerInterruptSource(IInterruptSource* pInterruptSource)
{
    assert(m_pTimerInterruptSource == nullptr);

    m_pTimerInterruptSource = pInterruptSource;
}

void InterruptController::UpdateCsr()
{
    const auto mideleg = m_pCsr->ReadUInt32(csr_addr_t::mideleg);
    const auto sideleg = m_pCsr->ReadUInt32(csr_addr_t::sideleg);

    xip_t pending = m_pCsr->ReadInterruptPending();

    // Timer interrupt
    pending.SetMember<xip_t::MTIP>(0);
    pending.SetMember<xip_t::STIP>(0);
    pending.SetMember<xip_t::UTIP>(0);

    if (m_pTimerInterruptSource->IsRequested())
    {
        if ((mideleg >> static_cast<int>(InterruptType::MachineTimer)) == 0)
        {
            pending.SetMember<xip_t::MTIP>(1);
        }
        else if ((sideleg >> static_cast<int>(InterruptType::SupervisorTimer)) == 0)
        {
            pending.SetMember<xip_t::STIP>(1);
        }
        else
        {
            pending.SetMember<xip_t::UTIP>(1);
        }
    }

    // External interrupt
    pending.SetMember<xip_t::MEIP>(0);
    pending.SetMember<xip_t::SEIP>(0);
    pending.SetMember<xip_t::UEIP>(0);

    if (m_pTimerInterruptSource->IsRequested())
    {
        if ((mideleg >> static_cast<int>(InterruptType::MachineExternal)) == 0)
        {
            pending.SetMember<xip_t::MEIP>(1);
        }
        else if ((sideleg >> static_cast<int>(InterruptType::SupervisorExternal)) == 0)
        {
            pending.SetMember<xip_t::SEIP>(1);
        }
        else
        {
            pending.SetMember<xip_t::UEIP>(1);
        }
    }

    m_pCsr->WriteInterruptPending(pending);
}

}}}
