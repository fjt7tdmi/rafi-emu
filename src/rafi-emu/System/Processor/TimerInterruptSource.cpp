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

#include <cassert>
#include <cstdint>

#include "TimerInterruptSource.h"

TimerInterruptSource::TimerInterruptSource(Csr* pCsr)
    : m_pCsr(pCsr)
{
}

bool TimerInterruptSource::IsRequested() const
{
    return m_Requested;
}

void TimerInterruptSource::Update()
{
    bool enable;
    bool pending; 

    switch (m_pCsr->GetPrivilegeLevel())
    {
    case PrivilegeLevel::Machine:
        enable = m_pCsr->ReadAs<xie_t>(csr_addr_t::mie).GetMember<xie_t::MTIE>() != 0;
        pending = m_pCsr->ReadAs<xip_t>(csr_addr_t::mip).GetMember<xip_t::MTIP>() != 0;
        break;
    case PrivilegeLevel::Supervisor:
        enable = m_pCsr->ReadAs<xie_t>(csr_addr_t::sie).GetMember<xie_t::STIE>() != 0;
        pending = m_pCsr->ReadAs<xip_t>(csr_addr_t::sip).GetMember<xip_t::STIP>() != 0;
        break;
    case PrivilegeLevel::User:
        enable = m_pCsr->ReadAs<xie_t>(csr_addr_t::uie).GetMember<xie_t::UTIE>() != 0;
        pending = m_pCsr->ReadAs<xip_t>(csr_addr_t::uip).GetMember<xip_t::UTIP>() != 0;
        break;
    default:
        std::abort();
    }

    m_Requested = enable && pending;
}
