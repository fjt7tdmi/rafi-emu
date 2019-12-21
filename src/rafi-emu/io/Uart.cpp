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

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstring>

#include <rafi/emu.h>

#include "Uart.h"

namespace rafi { namespace emu { namespace io {

void Uart::Read(void* pOutBuffer, size_t size, uint64_t address)
{
    RAFI_EMU_CHECK_ACCESS(address, size, GetSize());

    uint32_t value;

    if (size != sizeof(value))
    {
        RAFI_EMU_ERROR("Invalid access size (%zd byte).\n", size);
    }

    switch (address)
    {
    case Address_TxData:
        value = m_TxChars.empty() ? 0 : m_TxChars.back();
        break;
    case Address_RxData:
        value = m_RxChar;
        break;
    case Address_InterruptEnable:
        value = m_InterruptEnable.GetValue();
        break;
    case Address_InterruptPending:
        value = m_InterruptPending.GetValue();
        break;
    default:
        RAFI_EMU_NOT_IMPLEMENTED;
    }

    std::memcpy(pOutBuffer, &value, sizeof(value));
}

void Uart::Write(const void* pBuffer, size_t size, uint64_t address)
{
    RAFI_EMU_CHECK_ACCESS(address, size, GetSize());

    uint32_t value;

    if (size != sizeof(value))
    {
        RAFI_EMU_ERROR("Invalid access size (%zd byte).\n", size);
    }

    std::memcpy(&value, pBuffer, sizeof(uint32_t));

    switch (address)
    {
    case Address_TxData:
        m_TxChars.push_back(static_cast<char>(value));
        break;
    case Address_RxData:
        break;
    case Address_InterruptEnable:
        m_InterruptEnable.SetValue(value);
        break;
    case Address_InterruptPending:
        m_InterruptPending.SetValue(value);
        break;
    default:
        RAFI_EMU_NOT_IMPLEMENTED;
    }
}

int Uart::GetSize() const
{
    return RegisterSpaceSize;
}

bool Uart::IsInterruptRequested() const
{
    return false;
}

void Uart::ProcessCycle()
{
    //UpdateRx();
    PrintTx();

    m_Cycle++;
}

void Uart::UpdateRx()
{
    if (m_Cycle < InitialRxCycle)
    {
        return;
    }

    if ((m_Cycle - InitialRxCycle) % RxCycle == 0)
    {
        return;
    }

    int c = getchar();

    if (c == EOF)
    {
        return;
    }

    m_RxChar = static_cast<char>(c);
    m_InterruptPending.SetMember<InterruptPending::RXIP>(1);
}

void Uart::PrintTx()
{
    for (size_t i = m_PrintCount; i < m_TxChars.size(); i++)
    {
        char c = m_TxChars[i];

        if (std::isprint(c) || c == '\r' || c == '\n')
        {
            putchar(m_TxChars[i]);
        }
        else
        {
            printf("\\x%02x", c);
        }
    }

    m_PrintCount = m_TxChars.size();
}

}}}
