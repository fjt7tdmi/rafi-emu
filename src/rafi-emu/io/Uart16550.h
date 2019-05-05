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

#include <vector>

#include <rafi/emu.h>

#include "../io/IIo.h"

namespace rafi { namespace emu { namespace io {

/*
 * ns16550 emulation module.
 *
 * Restriction:
 *   - RX is not implemented.
 *   - TX/RX FIFO is not implemented. Characters written to data register will output to console immediately.
 */
class Uart16550 : public IIo
{
public:
    virtual void Read(void* pOutBuffer, size_t size, uint64_t address) override;
    virtual void Write(const void* pBuffer, size_t size, uint64_t address) override;
    virtual int GetSize() const override;
    virtual bool IsInterruptRequested() const override;

    void ProcessCycle();

private:
    // Register address
    static const int AddrData = 0;
    static const int AddrInterruptEnable = 1;
    static const int AddrInterruptIdent = 2; // Read
    static const int AddrFifoControl = 2; // Write
    static const int AddrLineControl = 3;
    static const int AddrModemControl = 4;
    static const int AddrLineStatus = 5;
    static const int AddrModemStatus = 6;
    static const int AddrScratch = 7;

    // Register space size
    static const int RegisterSpaceSize = 8;

    void PrintTx();

    uint8_t m_TxChar{ 0x0 };
    uint8_t m_InterruptEnable{ 0x0 };
    uint8_t m_InterruptIdent{ 0x1 };
    uint8_t m_FifoControl{ 0x0 };
    uint8_t m_LineControl{ 0x0 };
    uint8_t m_LineStatus{ 0x60 };
};

}}}
