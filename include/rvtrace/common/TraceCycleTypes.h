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

enum class NodeType : int32_t
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
    MemoryAccess32 = 10,
    MemoryAccess64 = 11,
    Io = 12,
    Memory = 13,
};

struct TraceCycleHeader
{
    // Byte offset to TraceCycleFooter from this TraceCycleHeader.
    int64_t footerOffset;

    // Number of TraceCycleMetaNode
    int32_t metaCount;

    int32_t reserved;
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

    int32_t reserved;

    // Byte offset from the head of TraceCycleHeader
    int64_t offset;

    // Byte size
    int64_t size;
};

struct BasicInfoNode
{
    int32_t cycle;
    int32_t opId;
    int32_t insn;
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
    int32_t regs[32];
};

struct IntReg64Node
{
    int64_t regs[32];
};

struct Pc32Node
{
    int32_t virtualPc;
    int32_t physicalPc;
};

struct Pc64Node
{
    int64_t virtualPc;
    int64_t physicalPc;
};

struct Csr32Node
{
    int32_t address;
    int32_t value;
};

struct Csr64Node
{
    int32_t address;
    int32_t reserved;
    int64_t value;
};

struct Trap32Node
{
    TrapType trapType;
    PrivilegeLevel from;
    PrivilegeLevel to;
    int32_t cause;
    int32_t trapValue;
    int32_t reserved;
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

struct MemoryAccess32Node
{
    MemoryAccessType accessType;
    int32_t size;
    int64_t value;
    int32_t virtualAddress;
    int32_t physicalAddress;
};

struct MemoryAccess64Node
{
    MemoryAccessType accessType;
    int32_t size;
    int64_t value;
    int64_t virtualAddress;
    int64_t physicalAddress;
};

struct IoNode
{
    int32_t hostIoValue;
    int32_t reserved;
};

}