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

#include "CsrAccessor.h"

using namespace rvtrace;

int32_t CsrAccessor::Read(int addr)
{
    auto csrAddr = static_cast<csr_addr_t>(addr);
    auto value = m_pCsr->Read(csrAddr);

    // set event
    m_ReadEvent.address = csrAddr;
    m_ReadEvent.value = value;
    m_ReadEventValid = true;

    return value;
}

void CsrAccessor::Write(int addr, int32_t value)
{
    auto csrAddr = static_cast<csr_addr_t>(addr);
    m_pCsr->Write(csrAddr, value);

    // set event
    m_WriteEvent = {csrAddr, value};
    m_WriteEventValid = true;
}

void CsrAccessor::ClearEvent()
{
    m_ReadEventValid = false;
    m_WriteEventValid = false;

    std::memset(&m_ReadEvent, 0, sizeof(m_ReadEvent));
    std::memset(&m_WriteEvent, 0, sizeof(m_WriteEvent));
}

void CsrAccessor::CopyReadEvent(CsrReadEvent* pOut) const
{
    std::memcpy(pOut, &m_ReadEvent, sizeof(*pOut));
}

void CsrAccessor::CopyWriteEvent(CsrWriteEvent* pOut) const
{
    std::memcpy(pOut, &m_WriteEvent, sizeof(*pOut));
}

bool CsrAccessor::IsReadEventExist() const
{
    return m_ReadEventValid;
}

bool CsrAccessor::IsWriteEventExist() const
{
    return m_WriteEventValid;
}
