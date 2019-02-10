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

#include "RvTypes.h"

namespace rvtrace {

const int NodeTypeMax = 32;

enum class NodeType : uint32_t
{
    BasicInfo = 0,
    FpReg = 1,
    IntReg32 = 2,
    IntReg64 = 3,
    Pc32 = 4,
    Pc64 = 5,
    Csr32 = 6,
    Csr64 = 7,
    Trap32 = 8,
    Trap64 = 9,
    MemoryAccess = 10,
    Memory = 11,
    Io = 12,
};

struct TraceCycleHeader
{
    // Byte offset to TraceCycleFooter from this TraceCycleHeader.
    int64_t footerOffset;

    // Number of TraceCycleMetaNode
    uint32_t metaCount;

    uint32_t reserved;
};

struct TraceCycleFooter
{
    // Byte offset to TraceCycleHeader from this TraceCycleFooter.
    // This value is positive and equal to TraceCycleHeader::footerOffset.
    int64_t headerOffset;
};

struct TraceCycleMetaNode
{
    NodeType nodeType;

    uint32_t reserved;

    // Byte offset from the head of TraceCycleHeader
    int64_t offset;

    // Byte size
    int64_t size;
};

struct BasicInfoNode
{
    uint32_t cycle;
    uint32_t opId;
    uint32_t insn;
    PrivilegeLevel privilegeLevel;
};

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

struct FpRegNode
{
    FpRegNodeUnion regs[32];
};

struct IntReg32Node
{
    uint32_t regs[32];
};

struct IntReg64Node
{
    int64_t regs[32];
};

struct Pc32Node
{
    uint32_t virtualPc;
    uint32_t physicalPc;
};

struct Pc64Node
{
    int64_t virtualPc;
    int64_t physicalPc;
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
    int64_t value;
};

struct Trap32Node
{
    TrapType trapType;
    PrivilegeLevel from;
    PrivilegeLevel to;
    uint32_t cause;
    uint32_t trapValue;
    uint32_t reserved;
};

struct Trap64Node
{
    TrapType trapType;
    PrivilegeLevel from;
    PrivilegeLevel to;
    int8_t cause;
    int8_t reserved[3];
    int64_t trapValue;
};

struct MemoryAccessNode
{
    MemoryAccessType accessType;
    uint32_t size;
    uint64_t value;
    uint64_t virtualAddress;
    uint64_t physicalAddress;
};

struct IoNode
{
    uint32_t hostIoValue;
    uint32_t reserved;
};

}