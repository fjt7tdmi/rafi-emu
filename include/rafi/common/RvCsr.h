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

enum class csr_addr_t : uint32_t
{
    // User Trap Setup
    ustatus = 0x000,
    uie = 0x004,
    utvec = 0x005,

    // User Trap Handling
    uscratch = 0x040,
    uepc = 0x041,
    ucause = 0x042,
    utval = 0x043,
    uip = 0x044,

    // User Floating-Point CSRs
    fflags = 0x001,
    frm = 0x002,
    fcsr = 0x003,

    // Supervisor Trap Setup
    sstatus = 0x100,
    sedeleg = 0x102,
    sideleg = 0x103,
    sie = 0x104,
    stvec = 0x105,
    scounteren = 0x106,

    // Supervisor Trap Handling
    sscratch = 0x140,
    sepc = 0x141,
    scause = 0x142,
    stval = 0x143,
    sip = 0x144,

    // Supervisor Protection and Translation
    satp = 0x180,

    // Machine Trap Setup
    mstatus = 0x300, // Machine Status
    misa = 0x301, // Machine ISA
    medeleg = 0x302, // Machine Exception Delegation
    mideleg = 0x303, // Machine Interrupt Delegation
    mie = 0x304, // Machine Interrupt Enable
    mtvec = 0x305, // Machine Trap Vector
    mcounteren = 0x306,

    // Machine Trap Handling
    mscratch = 0x340,
    mepc = 0x341,
    mcause = 0x342,
    mtval = 0x343,
    mip = 0x344, // Machine Interrupt Pending

    // Machine Protection and Translation
    pmpcfg0 = 0x3a0,
    pmpcfg1 = 0x3a1,
    pmpcfg2 = 0x3a2,
    pmpcfg3 = 0x3a3,

    pmpaddr_begin = 0x3b0,
    pmpaddr_end = 0x3c0,

    // Debug Trace Registers (Unimplemented)
    tselect = 0x7a0,
    tdata1 = 0x7a1,
    tdata2 = 0x7a2,
    tdata3 = 0x7a3,

    // Debug Mode Registers (Unimplemented)
    dcsr = 0x7b0,
    dpc = 0x7b1,
    dscratch = 0x7b2,

    // Machine Counter / Timers
    mhpmcounter_begin = 0xb00,
    mcycle = 0xb00,
    mtime = 0xb01,
    minstret = 0xb02,
    mhpmcounter3 = 0xb03,
    mhpmcounter4 = 0xb04,
    mhpmcounter5 = 0xb05,
    mhpmcounter6 = 0xb06,
    mhpmcounter7 = 0xb07,
    mhpmcounter8 = 0xb08,
    mhpmcounter9 = 0xb09,
    mhpmcounter10 = 0xb0a,
    mhpmcounter11 = 0xb0b,
    mhpmcounter12 = 0xb0c,
    mhpmcounter13 = 0xb0d,
    mhpmcounter14 = 0xb0e,
    mhpmcounter15 = 0xb0f,
    mhpmcounter16 = 0xb10,
    mhpmcounter17 = 0xb11,
    mhpmcounter18 = 0xb12,
    mhpmcounter19 = 0xb13,
    mhpmcounter20 = 0xb14,
    mhpmcounter21 = 0xb15,
    mhpmcounter22 = 0xb16,
    mhpmcounter23 = 0xb17,
    mhpmcounter24 = 0xb18,
    mhpmcounter25 = 0xb19,
    mhpmcounter26 = 0xb1a,
    mhpmcounter27 = 0xb1b,
    mhpmcounter28 = 0xb1c,
    mhpmcounter29 = 0xb1d,
    mhpmcounter30 = 0xb1e,
    mhpmcounter31 = 0xb1f,
    mhpmcounter_end = 0xb20,

    mhpmcounterh_begin = 0xb80,
    mcycleh = 0xb80,
    mtimeh = 0xb81,
    minstreth = 0xb82,
    mhpmcounter3h = 0xb83,
    mhpmcounter4h = 0xb84,
    mhpmcounter5h = 0xb85,
    mhpmcounter6h = 0xb86,
    mhpmcounter7h = 0xb87,
    mhpmcounter8h = 0xb88,
    mhpmcounter9h = 0xb89,
    mhpmcounter10h = 0xb8a,
    mhpmcounter11h = 0xb8b,
    mhpmcounter12h = 0xb8c,
    mhpmcounter13h = 0xb8d,
    mhpmcounter14h = 0xb8e,
    mhpmcounter15h = 0xb8f,
    mhpmcounter16h = 0xb90,
    mhpmcounter17h = 0xb91,
    mhpmcounter18h = 0xb92,
    mhpmcounter19h = 0xb93,
    mhpmcounter20h = 0xb94,
    mhpmcounter21h = 0xb95,
    mhpmcounter22h = 0xb96,
    mhpmcounter23h = 0xb97,
    mhpmcounter24h = 0xb98,
    mhpmcounter25h = 0xb99,
    mhpmcounter26h = 0xb9a,
    mhpmcounter27h = 0xb9b,
    mhpmcounter28h = 0xb9c,
    mhpmcounter29h = 0xb9d,
    mhpmcounter30h = 0xb9e,
    mhpmcounter31h = 0xb9f,
    mhpmcounterh_end = 0xba0,

    // User Counter / Timers
    hpmcounter_begin = 0xc00,
    cycle = 0xc00,
    time = 0xc01,
    instret = 0xc02,
    hpmcounter3 = 0xc03,
    hpmcounter4 = 0xc04,
    hpmcounter5 = 0xc05,
    hpmcounter6 = 0xc06,
    hpmcounter7 = 0xc07,
    hpmcounter8 = 0xc08,
    hpmcounter9 = 0xc09,
    hpmcounter10 = 0xc0a,
    hpmcounter11 = 0xc0b,
    hpmcounter12 = 0xc0c,
    hpmcounter13 = 0xc0d,
    hpmcounter14 = 0xc0e,
    hpmcounter15 = 0xc0f,
    hpmcounter16 = 0xc10,
    hpmcounter17 = 0xc11,
    hpmcounter18 = 0xc12,
    hpmcounter19 = 0xc13,
    hpmcounter20 = 0xc14,
    hpmcounter21 = 0xc15,
    hpmcounter22 = 0xc16,
    hpmcounter23 = 0xc17,
    hpmcounter24 = 0xc18,
    hpmcounter25 = 0xc19,
    hpmcounter26 = 0xc1a,
    hpmcounter27 = 0xc1b,
    hpmcounter28 = 0xc1c,
    hpmcounter29 = 0xc1d,
    hpmcounter30 = 0xc1e,
    hpmcounter31 = 0xc1f,
    hpmcounter_end = 0xc20,

    hpmcounterh_begin = 0xc80,
    cycleh = 0xc80,
    timeh = 0xc81,
    instreth = 0xc82,
    hpmcounter3h = 0xc83,
    hpmcounter4h = 0xc84,
    hpmcounter5h = 0xc85,
    hpmcounter6h = 0xc86,
    hpmcounter7h = 0xc87,
    hpmcounter8h = 0xc88,
    hpmcounter9h = 0xc89,
    hpmcounter10h = 0xc8a,
    hpmcounter11h = 0xc8b,
    hpmcounter12h = 0xc8c,
    hpmcounter13h = 0xc8d,
    hpmcounter14h = 0xc8e,
    hpmcounter15h = 0xc8f,
    hpmcounter16h = 0xc90,
    hpmcounter17h = 0xc91,
    hpmcounter18h = 0xc92,
    hpmcounter19h = 0xc93,
    hpmcounter20h = 0xc94,
    hpmcounter21h = 0xc95,
    hpmcounter22h = 0xc96,
    hpmcounter23h = 0xc97,
    hpmcounter24h = 0xc98,
    hpmcounter25h = 0xc99,
    hpmcounter26h = 0xc9a,
    hpmcounter27h = 0xc9b,
    hpmcounter28h = 0xc9c,
    hpmcounter29h = 0xc9d,
    hpmcounter30h = 0xc9e,
    hpmcounter31h = 0xc9f,
    hpmcounterh_end = 0xca0,

    // Machine Information Registers
    mvendorid = 0xf11,
    marchid = 0xf12,
    mimpid = 0xf13,
    mhartid = 0xf14,
};

// Defintion for each register
struct misa_t : public BitField64
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

    using MXL_RV32 = Member<31, 30>;
    using MXL_RV64 = Member<63, 62>;
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
struct xstatus_t : BitField64
{
    xstatus_t()
    {
    }

    xstatus_t(uint32_t value) : BitField64(value)
    {
    }

    using SD_RV64    = Member<63>;   // Status Dirty
    using SD_RV32    = Member<31>;   // Status Dirty

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

    static const uint64_t SupervisorMask_RV64 = SD_RV64::Mask | MXR::Mask | SUM::Mask | XS::Mask | FS::Mask | SPP::Mask | SPIE::Mask | UPIE::Mask | SIE::Mask | UIE::Mask;
    static const uint64_t SupervisorMask_RV32 = SD_RV32::Mask | MXR::Mask | SUM::Mask | XS::Mask | FS::Mask | SPP::Mask | SPIE::Mask | UPIE::Mask | SIE::Mask | UIE::Mask;
    static const uint64_t UserMask = UPIE::Mask | UIE::Mask;
};

// mtvec, stvec, utvec
struct xtvec_t : BitField64
{
    xtvec_t()
    {
    }

    xtvec_t(uint32_t value) : BitField64(value)
    {
    }

    using BASE_RV32 = Member<31, 2>;
    using BASE_RV64 = Member<63, 2>;
    using MODE = Member<1, 0>;

    enum class Mode : uint32_t
    {
        Directed = 0,
        Vectored = 1,
    };
};

// mip, sip, uip
struct xip_t : BitField64
{
    xip_t() : BitField64(0)
    {
    }

    xip_t(uint64_t value) : BitField64(value)
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

    static const uint64_t WriteMask = MEIP::Mask | SEIP::Mask | UEIP::Mask | MSIP::Mask | SSIP::Mask | USIP::Mask;

    static const uint64_t UserMask = UEIP::Mask | UTIP::Mask | USIP::Mask;
    static const uint64_t SupervisorMask = SEIP::Mask | STIP::Mask | SSIP::Mask | UserMask;
    static const uint64_t MachineMask =  MEIP::Mask | MTIP::Mask | MSIP::Mask | SupervisorMask;
};

// mie, sie, uie
struct xie_t : BitField64
{
    xie_t() : BitField64(0)
    {
    }

    xie_t(uint64_t value) : BitField64(value)
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

    static const uint64_t WriteMask = MEIE::Mask | SEIE::Mask | UEIE::Mask | MSIE::Mask | SSIE::Mask | USIE::Mask;

    static const uint64_t UserMask = UEIE::Mask | UTIE::Mask | USIE::Mask;
    static const uint64_t SupervisorMask = SEIE::Mask | STIE::Mask | SSIE::Mask | UserMask;
    static const uint64_t MachineMask = MEIE::Mask | MTIE::Mask | MSIE::Mask | SupervisorMask;
};

// satp
struct satp_t : BitField64
{
    satp_t() : BitField64(0)
    {
    }

    satp_t(uint64_t value) : BitField64(value)
    {
    }

    using MODE_RV64 = Member<63, 60>;
    using ASID_RV64 = Member<59, 44>;
    using PPN_RV64 = Member<43, 0>;

    using MODE_RV32 = Member<31>;
    using ASID_RV32 = Member<30, 22>;
    using PPN_RV32 = Member<21, 0>;
};

}
