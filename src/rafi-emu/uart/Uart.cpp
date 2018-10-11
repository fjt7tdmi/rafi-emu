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
#include <cstdio>

#include <rafi/Macro.h>

#include "Uart.h"

namespace rafi { namespace uart {

int8_t Uart::GetInt8(int address)
{
    return static_cast<int8_t>(Read(address, sizeof(int8_t)));
}

void Uart::SetInt8(int address, int8_t value)
{
    Write(address, static_cast<int32_t>(value), sizeof(int8_t));
}

int16_t Uart::GetInt16(int address)
{
    return static_cast<int16_t>(Read(address, sizeof(int16_t)));
}

void Uart::SetInt16(int address, int16_t value)
{
    Write(address, static_cast<int32_t>(value), sizeof(int16_t));
}

int32_t Uart::GetInt32(int address)
{
    return Read(address, sizeof(int32_t));
}

void Uart::SetInt32(int address, int32_t value)
{
    Write(address, static_cast<int32_t>(value), sizeof(int32_t));
}

void Uart::ProcessCycle()
{
    //UpdateRx();
    PrintTx();

    m_Cycle++;
}

int32_t Uart::Read(int address, int size)
{
    if (!(0 <= address && 0 <= size && address + size - 1 < RegSize))
    {
        ABORT();
    }

    switch (address)
    {
    case Address_TxData:
        return m_TxChars.empty() ? 0 : m_TxChars.back();
    case Address_RxData:
        return m_RxChar;
    case Address_InterruptEnable:
        return m_InterruptEnable.GetInt32();
    case Address_InterruptPending:
        return m_InterruptPending.GetInt32();
    default:
        ABORT();
    }
}

void Uart::Write(int address, int32_t value, int size)
{
    if (!(0 <= address && 0 <= size && address + size - 1 < RegSize))
    {
        ABORT();
    }

    switch (address)
    {
    case Address_TxData:
        m_TxChars.push_back(static_cast<char>(value));
    case Address_RxData:
        break;
    case Address_InterruptEnable:
        m_InterruptEnable.Set(value);
    case Address_InterruptPending:
        m_InterruptPending.Set(value);
    default:
        ABORT();
    }
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
        putchar(m_TxChars[i]);
    }

    m_PrintCount = m_TxChars.size();
}

}}
