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

class VirtIo : public IIo
{
public:
    virtual void Read(void* pOutBuffer, size_t size, uint64_t address) override;
    virtual void Write(const void* pBuffer, size_t size, uint64_t address) override;
    virtual int GetSize() const override;
    virtual bool IsInterruptRequested() const override;

private:
    // Register address
    static const uint64_t RegSpaceSize = 0x1000;

    static const uint64_t AddrMagicValue = 0x0;
    static const uint64_t AddrVersion    = 0x4;
    static const uint64_t AddrVendorId   = 0xc;

    static const uint32_t MagicValue    = 0x74726976; // virt
    static const uint32_t Version       = 1;
    static const uint32_t VendorId      = 0x49464152; // RAFI
};

}}}
