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

#include "Uart16550.h"

namespace rafi { namespace emu { namespace io {

void Uart16550::Read(void* pOutBuffer, size_t size, uint64_t address)
{
    RAFI_EMU_CHECK_ACCESS(address, size, GetSize());

    if (size != 1)
    {
        RAFI_EMU_ERROR("[Uart16550] Invalid read size (%zd byte).\n", size);
    }

    RAFI_EMU_ERROR("[Uart16550] Read to register %llu is invalid or unimplmented.\n", static_cast<unsigned long long>(address));
}

void Uart16550::Write(const void* pBuffer, size_t size, uint64_t address)
{
    RAFI_EMU_CHECK_ACCESS(address, size, GetSize());

    char value;

    if (size != sizeof(value))
    {
        RAFI_EMU_ERROR("[Uart16550] Invalid write size (%zd byte).\n", size);
    }

    std::memcpy(&value, pBuffer, sizeof(value));

    switch (address)
    {
    case AddrData:
        m_TxChar = value;
        break;
    default:
        RAFI_EMU_ERROR("[Uart16550] Write to register %llu is invalid or unimplmented.\n", static_cast<unsigned long long>(address));
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
}

void Uart16550::PrintTx()
{
    if (m_TxChar != '\0')
    {
        putchar(m_TxChar);
    }

    m_TxChar = '\0';
}

}}}
