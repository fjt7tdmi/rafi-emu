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

#include <cstddef>
#include <cstdint>

#include <rafi/common.h>

namespace rafi { namespace trace {

const uint16_t NodeId_BA = 0x4142; // BASIC
const uint16_t NodeId_BR = 0x5242; // BREAK
const uint16_t NodeId_IN = 0x4e49; // INT
const uint16_t NodeId_FP = 0x5046; // FP
const uint16_t NodeId_IO = 0x4f49; // IO
const uint16_t NodeId_OP = 0x504f; // OP
const uint16_t NodeId_TR = 0x5254; // TRAP
const uint16_t NodeId_MA = 0x414d; // MA

union FpRegUnion
{
    struct
    {
        uint64_t value;
    } u64;
    struct
    {
        float value;
        uint32_t zero;
    } f32;
    struct
    {
        double value;
    } f64;
};

struct NodeHeader
{
    uint16_t nodeId;
    uint16_t reserved;
    uint32_t nodeSize;
};

struct NodeBasic
{
    uint32_t cycle;
    XLEN xlen;
    uint64_t pc;
};

struct NodeIntReg32
{
    uint32_t regs[IntRegCount];
};

struct NodeIntReg64
{
    uint64_t regs[IntRegCount];
};

struct NodeFpReg
{
    FpRegUnion regs[FpRegCount];
};

struct NodeIo
{
    uint32_t hostIo;
    uint32_t reserved;
};

struct NodeOpEvent
{
    uint32_t insn;
    PrivilegeLevel priv;
};

struct NodeTrapEvent
{
    TrapType trapType;
    PrivilegeLevel from;
    PrivilegeLevel to;
    uint32_t cause;
    uint64_t trapValue;
};

struct NodeMemoryEvent
{
    MemoryAccessType accessType;
    uint32_t size;
    uint64_t value;
    uint64_t vaddr;
    uint64_t paddr;
};

// ============================================================================
// Binary Trace v1

union FpRegNodeUnion
{
    struct
    {
        uint64_t value;
    } u64;
    struct
    {
        float value;
        uint32_t zero;
    } f32;
    struct
    {
        double value;
    } f64;
};

struct Csr32Node
{
    uint32_t address;
    uint32_t value;
};

struct Csr64Node
{
    uint32_t address;
    uint32_t reserved;
    uint64_t value;
};

}}
