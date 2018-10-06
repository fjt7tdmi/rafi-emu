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

#include <cstring>

#include "../../Common/BitField.h"

#include "ProcessorException.h"

using namespace rvtrace;

enum XLEN
{
    XLEN32 = 1,
    XLEN64 = 2,
    XLEN128 = 3,
};

// Defintion for each register
struct misa_t : public BitField
{
    misa_t() : BitField(0)
    {
    }

    using A = BitFieldMember<0>;
    using B = BitFieldMember<1>;
    using C = BitFieldMember<2>;
    using D = BitFieldMember<3>;
    using E = BitFieldMember<4>;
    using F = BitFieldMember<5>;
    using G = BitFieldMember<6>;
    using H = BitFieldMember<7>;
    using I = BitFieldMember<8>;
    using J = BitFieldMember<9>;
    using K = BitFieldMember<10>;
    using L = BitFieldMember<11>;
    using M = BitFieldMember<12>;
    using N = BitFieldMember<13>;
    using O = BitFieldMember<14>;
    using P = BitFieldMember<15>;
    using Q = BitFieldMember<16>;
    using R = BitFieldMember<17>;
    using S = BitFieldMember<18>;
    using T = BitFieldMember<19>;
    using U = BitFieldMember<20>;
    using V = BitFieldMember<21>;
    using W = BitFieldMember<22>;
    using X = BitFieldMember<23>;
    using Y = BitFieldMember<24>;
    using Z = BitFieldMember<25>;

    using XLEN = BitFieldMember<31, 30>;
};

struct mvendorid
{
    static const int NonCommercial = 0;
};

struct marchid
{
    static const int NotImplemented = 0;
};

struct mimpid
{
    static const int NotImplemented = 0;
};

// mstatus, sstatus, ustatus
struct xstatus_t : BitField
{
    xstatus_t() : BitField(0)
    {
    }

    xstatus_t(int32_t value) : BitField(value)
    {
    }

    using SD    = BitFieldMember<31>;   // Status Dirty

    using TSR   = BitFieldMember<22>;   // Trap SRET
    using TW    = BitFieldMember<21>;   // Timeout Wait
    using TVM   = BitFieldMember<20>;   // Trap Virtual Memory
    using MXR   = BitFieldMember<19>;   // Make eXecutable Readable
    using SUM   = BitFieldMember<18>;   // permit Supervisor User Memory access
    using MPRV  = BitFieldMember<17>;   // Modify PRiVilege

    using XS    = BitFieldMember<16, 15>;   // additional user-mode eXtensions Status
    using FS    = BitFieldMember<14, 13>;   // Floating-point Status

    using MPP   = BitFieldMember<12, 11>;   // Machine Previous Priviledged mode
    using SPP   = BitFieldMember<8, 8>;     // Supervisor Previous Priviledged mode

    using MPIE  = BitFieldMember<7>;    // Supervisor Previous Interrupt Enable
    using SPIE  = BitFieldMember<5>;    // Machine Previous Interrupt Enable
    using UPIE  = BitFieldMember<4>;    // User Previous Interrupt Enable

    using MIE   = BitFieldMember<3>;    // Supervisor Interrupt Enable
    using SIE   = BitFieldMember<1>;    // Machine Interrupt Enable
    using UIE   = BitFieldMember<0>;    // User Interrupt Enable

    static const int32_t SupervisorMask = SD::Mask | MXR::Mask | SUM::Mask | XS::Mask | FS::Mask | SPP::Mask | SPIE::Mask | UPIE::Mask | SIE::Mask | UIE::Mask;
    static const int32_t UserMask = UPIE::Mask | UIE::Mask;
};

// mtvec, stvec, utvec
struct xtvec_t : BitField
{
    xtvec_t() : BitField(0)
    {
    }

    xtvec_t(int32_t value) : BitField(value)
    {
    }

    using BASE = BitFieldMember<31, 2>;
    using MODE = BitFieldMember<1, 0>;

    enum class Mode : int32_t
    {
        Directed = 0,
        Vectored = 1,
    };
};

// mip, sip, uip
struct xip_t : BitField
{
    xip_t() : BitField(0)
    {
    }

    xip_t(int32_t value) : BitField(value)
    {
    }

    using MEIP = BitFieldMember<11>;    // Machine External Interrupt Pending
    using SEIP = BitFieldMember<9>;     // Supervisor External Interrupt Pending
    using UEIP = BitFieldMember<8>;     // User External Interrupt Pending
    using MTIP = BitFieldMember<7>;     // Machine Timer Interrupt Pending
    using STIP = BitFieldMember<5>;     // Supervisor Timer Interrupt Pending
    using UTIP = BitFieldMember<4>;     // User Timer Interrupt Pending
    using MSIP = BitFieldMember<3>;     // Machine Software Interrupt Pending
    using SSIP = BitFieldMember<1>;     // Supervisor Software Interrupt Pending
    using USIP = BitFieldMember<0>;     // User Software Interrupt Pending

    static const int32_t WriteMask = MEIP::Mask | SEIP::Mask | UEIP::Mask | MSIP::Mask | SSIP::Mask | USIP::Mask;
    static const int32_t SupervisorMask = SEIP::Mask | UEIP::Mask | STIP::Mask | UTIP::Mask | SSIP::Mask | USIP::Mask;
    static const int32_t UserMask = UEIP::Mask | UTIP::Mask | USIP::Mask;
};

// mie, sie, uie
struct xie_t : BitField
{
    xie_t() : BitField(0)
    {
    }

    xie_t(int32_t value) : BitField(value)
    {
    }

    using MEIE = BitFieldMember<11>;    // Machine External Interrupt Enable
    using SEIE = BitFieldMember<9>;     // Supervisor External Interrupt Enable
    using UEIE = BitFieldMember<8>;     // User External Interrupt Enable
    using MTIE = BitFieldMember<7>;     // Machine Timer Interrupt Enable
    using STIE = BitFieldMember<5>;     // Supervisor Timer Interrupt Enable
    using UTIE = BitFieldMember<4>;     // User Timer Interrupt Enable
    using MSIE = BitFieldMember<3>;     // Machine Software Interrupt Enable
    using SSIE = BitFieldMember<1>;     // Supervisor Software Interrupt Enable
    using USIE = BitFieldMember<0>;     // User Software Interrupt Enable

    static const int32_t WriteMask = MEIE::Mask | SEIE::Mask | UEIE::Mask | MSIE::Mask | SSIE::Mask | USIE::Mask;
    static const int32_t SupervisorMask = SEIE::Mask | UEIE::Mask | STIE::Mask | UTIE::Mask | SSIE::Mask | USIE::Mask;
    static const int32_t UserMask = UEIE::Mask | UTIE::Mask | USIE::Mask;
};

// satp
struct satp_t : BitField
{
    satp_t() : BitField(0)
    {
    }

    satp_t(int32_t value) : BitField(value)
    {
    }

    using MODE = BitFieldMember<31>;
    using ASID = BitFieldMember<30, 22>;
    using PPN = BitFieldMember<21, 0>;

    enum class Mode : int32_t
    {
        Bare = 0,
        Sv32 = 1,
    };
};
