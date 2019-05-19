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

#include <cstring>

#include <rafi/emu.h>

#include "VirtIo.h"

namespace rafi { namespace emu { namespace io {

void VirtIo::Read(void* pOutBuffer, size_t size, uint64_t address)
{
    RAFI_EMU_CHECK_ACCESS(address, size, GetSize());

    uint32_t value;

    if (size != sizeof(value))
    {
        RAFI_EMU_ERROR("[VirtIo] Invalid read size (%zd) byte).\n", size);
    }

    switch (address)
    {
    case AddrMagicValue:
        value = MagicValue;
        break;
    case AddrVersion:
        value = Version;
        break;
    case AddrVendorId:
        value = VendorId;
        break;
    default:
        value = 0;
        break;
    }

    std::memcpy(pOutBuffer, &value, sizeof(value));
}

void VirtIo::Write(const void* pBuffer, size_t size, uint64_t address)
{
    RAFI_EMU_CHECK_ACCESS(address, size, GetSize());
}

int VirtIo::GetSize() const
{
    return RegSpaceSize;
}

bool VirtIo::IsInterruptRequested() const
{
    return false;
}

}}}
