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

#include <rafi/emu.h>

#include "Trap.h"

namespace rafi { namespace emu { namespace cpu {

enum XLEN
{
    XLEN32 = 1,
    XLEN64 = 2,
    XLEN128 = 3,
};

// Defintion for each register
struct misa_t : public BitField32
{
    misa_t()
    {
    }

    using A = Member<0>;
    using B = Member<1>;
    using C = Member<2>;
    using D = Member<3>;
    using E = Member<4>;
    using F = Member<5>;
    using G = Member<6>;
    using H = Member<7>;
    using I = Member<8>;
    using J = Member<9>;
    using K = Member<10>;
    using L = Member<11>;
    using M = Member<12>;
    using N = Member<13>;
    using O = Member<14>;
    using P = Member<15>;
    using Q = Member<16>;
    using R = Member<17>;
    using S = Member<18>;
    using T = Member<19>;
    using U = Member<20>;
    using V = Member<21>;
    using W = Member<22>;
    using X = Member<23>;
    using Y = Member<24>;
    using Z = Member<25>;

    using XLEN = Member<31, 30>;
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

// fcsr
struct fcsr_t : BitField32
{
    fcsr_t()
    {
    }

    fcsr_t(uint32_t value) : BitField32(value)
    {
    }

    using RM    = Member<7, 5>; // Rounding Mode
    using AE    = Member<4, 0>; // Accrued Exceptions

    using NV    = Member<4>;    // Invalid Operation
    using DZ    = Member<3>;    // Divide by Zero
    using OF    = Member<2>;    // Overflow
    using UF    = Member<1>;    // Underflow
    using NX    = Member<0>;    // Inexact

    static const uint32_t UserMask = RM::Mask | AE::Mask;
};

// mstatus, sstatus, ustatus
struct xstatus_t : BitField32
{
    xstatus_t()
    {
    }

    xstatus_t(uint32_t value) : BitField32(value)
    {
    }

    using SD    = Member<31>;   // Status Dirty

    using TSR   = Member<22>;   // Trap SRET
    using TW    = Member<21>;   // Timeout Wait
    using TVM   = Member<20>;   // Trap Virtual Memory
    using MXR   = Member<19>;   // Make eXecutable Readable
    using SUM   = Member<18>;   // permit Supervisor User Memory access
    using MPRV  = Member<17>;   // Modify PRiVilege

    using XS    = Member<16, 15>;   // additional user-mode eXtensions Status
    using FS    = Member<14, 13>;   // Floating-point Status

    using MPP   = Member<12, 11>;   // Machine Previous Priviledged mode
    using SPP   = Member<8, 8>;     // Supervisor Previous Priviledged mode

    using MPIE  = Member<7>;    // Supervisor Previous Interrupt Enable
    using SPIE  = Member<5>;    // Machine Previous Interrupt Enable
    using UPIE  = Member<4>;    // User Previous Interrupt Enable

    using MIE   = Member<3>;    // Supervisor Interrupt Enable
    using SIE   = Member<1>;    // Machine Interrupt Enable
    using UIE   = Member<0>;    // User Interrupt Enable

    static const uint32_t SupervisorMask = SD::Mask | MXR::Mask | SUM::Mask | XS::Mask | FS::Mask | SPP::Mask | SPIE::Mask | UPIE::Mask | SIE::Mask | UIE::Mask;
    static const uint32_t UserMask = UPIE::Mask | UIE::Mask;
};

// mtvec, stvec, utvec
struct xtvec_t : BitField32
{
    xtvec_t()
    {
    }

    xtvec_t(uint32_t value) : BitField32(value)
    {
    }

    using BASE = Member<31, 2>;
    using MODE = Member<1, 0>;

    enum class Mode : uint32_t
    {
        Directed = 0,
        Vectored = 1,
    };
};

// mip, sip, uip
struct xip_t : BitField32
{
    xip_t() : BitField32(0)
    {
    }

    xip_t(uint32_t value) : BitField32(value)
    {
    }

    using MEIP = Member<11>;    // Machine External Interrupt Pending
    using SEIP = Member<9>;     // Supervisor External Interrupt Pending
    using UEIP = Member<8>;     // User External Interrupt Pending
    using MTIP = Member<7>;     // Machine Timer Interrupt Pending
    using STIP = Member<5>;     // Supervisor Timer Interrupt Pending
    using UTIP = Member<4>;     // User Timer Interrupt Pending
    using MSIP = Member<3>;     // Machine Software Interrupt Pending
    using SSIP = Member<1>;     // Supervisor Software Interrupt Pending
    using USIP = Member<0>;     // User Software Interrupt Pending

    static const uint32_t WriteMask = MEIP::Mask | SEIP::Mask | UEIP::Mask | MSIP::Mask | SSIP::Mask | USIP::Mask;

    static const uint32_t UserMask = UEIP::Mask | UTIP::Mask | USIP::Mask;
    static const uint32_t SupervisorMask = SEIP::Mask | STIP::Mask | SSIP::Mask | UserMask;
    static const uint32_t MachineMask =  MEIP::Mask | MTIP::Mask | MSIP::Mask | SupervisorMask;
};

// mie, sie, uie
struct xie_t : BitField32
{
    xie_t() : BitField32(0)
    {
    }

    xie_t(uint32_t value) : BitField32(value)
    {
    }

    using MEIE = Member<11>;    // Machine External Interrupt Enable
    using SEIE = Member<9>;     // Supervisor External Interrupt Enable
    using UEIE = Member<8>;     // User External Interrupt Enable
    using MTIE = Member<7>;     // Machine Timer Interrupt Enable
    using STIE = Member<5>;     // Supervisor Timer Interrupt Enable
    using UTIE = Member<4>;     // User Timer Interrupt Enable
    using MSIE = Member<3>;     // Machine Software Interrupt Enable
    using SSIE = Member<1>;     // Supervisor Software Interrupt Enable
    using USIE = Member<0>;     // User Software Interrupt Enable

    static const uint32_t WriteMask = MEIE::Mask | SEIE::Mask | UEIE::Mask | MSIE::Mask | SSIE::Mask | USIE::Mask;

    static const uint32_t UserMask = UEIE::Mask | UTIE::Mask | USIE::Mask;
    static const uint32_t SupervisorMask = SEIE::Mask | STIE::Mask | SSIE::Mask | UserMask;
    static const uint32_t MachineMask = MEIE::Mask | MTIE::Mask | MSIE::Mask | SupervisorMask;
};

// satp
struct satp_t : BitField32
{
    satp_t() : BitField32(0)
    {
    }

    satp_t(uint32_t value) : BitField32(value)
    {
    }

    using MODE = Member<31>;
    using ASID = Member<30, 22>;
    using PPN = Member<21, 0>;

    enum class Mode : uint32_t
    {
        Bare = 0,
        Sv32 = 1,
    };
};

}}}
