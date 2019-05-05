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
#include <cinttypes>
#include <cstdio>
#include <cstring>

#include <rafi/emu.h>

#include "Uart16550.h"

namespace rafi { namespace emu { namespace io {

void Uart16550::Read(void* pOutBuffer, size_t size, uint64_t address)
{
    RAFI_EMU_CHECK_ACCESS(address, size, GetSize());

    uint8_t value = 0;

    if (size != sizeof(value))
    {
        RAFI_EMU_ERROR("[Uart16550] Invalid read size. (address: %llu, size: %zd byte).\n", static_cast<unsigned long long>(address), size);
    }

    switch (address)
    {
    case AddrLineStatus:
        value = m_LineStatus;
        break;
    default:
        RAFI_EMU_ERROR("[Uart16550] Read to register %llu is invalid or unimplmented.\n", static_cast<unsigned long long>(address));
    }

    std::memcpy(pOutBuffer, &value, sizeof(value));
}

void Uart16550::Write(const void* pBuffer, size_t size, uint64_t address)
{
    RAFI_EMU_CHECK_ACCESS(address, size, GetSize());

    uint8_t value;

    if (size != sizeof(value))
    {
        RAFI_EMU_ERROR("[Uart16550] Invalid write size. (address: %llu, size: %zd byte).\n", static_cast<unsigned long long>(address), size);
    }

    std::memcpy(&value, pBuffer, sizeof(value));

    switch (address)
    {
    case AddrData:
        if ((m_LineControl & 0x80) == 0)
        {
            m_TxChar = value;
        }
        else
        {
            // Ignore writing to Divisor Latch
        }
        break;
    case AddrInterruptEnable:
        if ((m_LineControl & 0x80) == 0)
        {
            m_InterruptEnable = value;
        }
        else
        {
            // Ignore writing to Divisor Latch
        }
        break;
    case AddrFifoControl:
        m_FifoControl = value;
        break;
    case AddrLineControl:
        m_LineControl = value;
        break;
    default:
        RAFI_EMU_ERROR("[Uart16550] Write to register %llu is invalid or unimplmented. (value: 0x%02x)\n", static_cast<unsigned long long>(address), value);
    }
}

int Uart16550::GetSize() const
{
    return RegisterSpaceSize;
}

bool Uart16550::IsInterruptRequested() const
{
    return false;
}

void Uart16550::ProcessCycle()
{
    PrintTx();

    if (m_InterruptEnable != 0)
    {
        RAFI_EMU_ERROR("[Uart16550] Interrupt is not implemented. (but, the value of IE is 0x%02x)\n", m_InterruptEnable);
    }
}

void Uart16550::PrintTx()
{
    if (m_TxChar != 0)
    {
        putchar(static_cast<char>(m_TxChar));
    }

    m_TxChar = 0;
}

}}}
