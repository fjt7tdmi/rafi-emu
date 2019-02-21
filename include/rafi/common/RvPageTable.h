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

#include "BitField.h"

namespace rafi {

class PageTableEntrySv32 : public BitField32
{
public:
    explicit PageTableEntrySv32(uint32_t value)
        : BitField32(value)
    {
    }

    using V = Member<0>; // Valid
    using R = Member<1>; // Read
    using W = Member<2>; // Write
    using E = Member<3>; // Execute
    using U = Member<4>; // User
    using G = Member<5>; // Global
    using A = Member<6>; // Accessed
    using D = Member<7>; // Dirty
    using RSW = Member<9, 8>; // Reserved for SoftWare

    using PPN0 = Member<19, 10>;
    using PPN1 = Member<31, 20>;
};

class PageTableEntrySv39 : public BitField64
{
public:
    explicit PageTableEntrySv39(uint64_t value)
        : BitField64(value)
    {
    }

    using V = Member<0>; // Valid
    using R = Member<1>; // Read
    using W = Member<2>; // Write
    using E = Member<3>; // Execute
    using U = Member<4>; // User
    using G = Member<5>; // Global
    using A = Member<6>; // Accessed
    using D = Member<7>; // Dirty
    using RSW = Member<9, 8>; // Reserved for SoftWare

    using PPN0 = Member<18, 10>;
    using PPN1 = Member<27, 19>;
    using PPN2 = Member<53, 28>;
};

class PageTableEntrySv48 : public BitField64
{
public:
    explicit PageTableEntrySv48(uint64_t value)
        : BitField64(value)
    {
    }

    using V = Member<0>; // Valid
    using R = Member<1>; // Read
    using W = Member<2>; // Write
    using E = Member<3>; // Execute
    using U = Member<4>; // User
    using G = Member<5>; // Global
    using A = Member<6>; // Accessed
    using D = Member<7>; // Dirty
    using RSW = Member<9, 8>; // Reserved for SoftWare

    using PPN0 = Member<18, 10>;
    using PPN1 = Member<27, 19>;
    using PPN2 = Member<36, 28>;
    using PPN3 = Member<53, 37>;
};

class PhysicalAddressSv32 : public BitField64
{
public:
    explicit PhysicalAddressSv32(uint64_t value)
        : BitField64(static_cast<uint64_t>(value))
    {
    }

    using Offset = Member<11, 0>;
    using PPN = Member<33, 12>;
    using PPN0 = Member<21, 12>;
    using PPN1 = Member<33, 22>;
};

class PhysicalAddressSv39 : public BitField64
{
public:
    explicit PhysicalAddressSv39(uint64_t value)
        : BitField64(static_cast<uint64_t>(value))
    {
    }

    using Offset = Member<11, 0>;
    using PPN = Member<55, 12>;
    using PPN0 = Member<20, 12>;
    using PPN1 = Member<29, 21>;
    using PPN2 = Member<55, 30>;
};

class PhysicalAddressSv48 : public BitField64
{
public:
    explicit PhysicalAddressSv48(uint64_t value)
        : BitField64(static_cast<uint64_t>(value))
    {
    }

    using Offset = Member<11, 0>;
    using PPN = Member<55, 12>;
    using PPN0 = Member<20, 12>;
    using PPN1 = Member<29, 21>;
    using PPN2 = Member<38, 30>;
    using PPN3 = Member<55, 39>;
};

class VirtualAddressSv32 : public BitField32
{
public:
    explicit VirtualAddressSv32(uint32_t value)
        : BitField32(static_cast<uint32_t>(value))
    {
    }

    using Offset = Member<11, 0>;
    using VPN0 = Member<21, 12>;
    using VPN1 = Member<31, 22>;

    using Offset_L1 = Member<21, 0>;
};

class VirtualAddressSv39 : public BitField64
{
public:
    explicit VirtualAddressSv39(uint64_t value)
        : BitField64(static_cast<uint64_t>(value))
    {
    }

    using Offset = Member<11, 0>;
    using VPN0 = Member<20, 12>;
    using VPN1 = Member<29, 21>;
    using VPN2 = Member<38, 30>;
};

class VirtualAddressSv48 : public BitField64
{
public:
    explicit VirtualAddressSv48(uint64_t value)
        : BitField64(static_cast<uint64_t>(value))
    {
    }

    using Offset = Member<11, 0>;
    using VPN0 = Member<20, 12>;
    using VPN1 = Member<29, 21>;
    using VPN2 = Member<38, 30>;
    using VPN3 = Member<47, 39>;
};

}
