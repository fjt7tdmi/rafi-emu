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

#include <rafi/Common.h>
#include "InterruptController.h"

namespace rafi { namespace cpu {

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

    bool enableInStatus;
    xie_t enable;
    xip_t pending;

    switch (m_pCsr->GetPrivilegeLevel())
    {
    case PrivilegeLevel::Machine:
        enableInStatus = m_pCsr->ReadStatus().GetMember<xstatus_t::MIE>();
        enable = m_pCsr->ReadInterruptEnable().GetWithMask(xie_t::MachineMask);
        pending = m_pCsr->ReadInterruptPending().GetWithMask(xie_t::MachineMask);
        break;
    case PrivilegeLevel::Supervisor:
        enableInStatus = m_pCsr->ReadStatus().GetMember<xstatus_t::SIE>();
        enable = m_pCsr->ReadInterruptEnable().GetWithMask(xie_t::SupervisorMask);
        pending = m_pCsr->ReadInterruptPending().GetWithMask(xie_t::SupervisorMask);
        break;
    case PrivilegeLevel::User:
        enableInStatus = m_pCsr->ReadStatus().GetMember<xstatus_t::UIE>();
        enable = m_pCsr->ReadInterruptEnable().GetWithMask(xie_t::UserMask);
        pending = m_pCsr->ReadInterruptPending().GetWithMask(xie_t::UserMask);
        break;
    default:
        ABORT();
    }

    const int32_t value = enable.GetInt32() & pending.GetInt32();

    m_IsRequested = enableInStatus && (value != 0);
    m_InterruptType = static_cast<InterruptType>(NumberOfTrainingZero(value));
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
    xip_t pending = m_pCsr->ReadInterruptPending();

    if (m_pTimerInterruptSource->IsRequested())
    {
        pending.SetMember<xip_t::UTIP>(1);
        pending.SetMember<xip_t::STIP>(1);
        pending.SetMember<xip_t::MTIP>(1);
    }
    else
    {
        pending.SetMember<xip_t::UTIP>(0);
        pending.SetMember<xip_t::STIP>(0);
        pending.SetMember<xip_t::MTIP>(0);
    }

    if (m_pExternalInterruptSource->IsRequested())
    {
        pending.SetMember<xip_t::UEIP>(1);
        pending.SetMember<xip_t::SEIP>(1);
        pending.SetMember<xip_t::MEIP>(1);
    }
    else
    {
        pending.SetMember<xip_t::UEIP>(0);
        pending.SetMember<xip_t::SEIP>(0);
        pending.SetMember<xip_t::MEIP>(0);
    }

    m_pCsr->WriteInterruptPending(pending);
}

}}
