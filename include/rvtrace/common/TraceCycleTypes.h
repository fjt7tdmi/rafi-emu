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

enum class NodeType : int32_t
{
    BasicInfo = 1,
    Pc32 = 2,
    Pc64 = 3,
    IntReg32 = 4,
    IntReg64 = 5,
    Csr32 = 6,
    Csr64 = 7,
    Trap32 = 8,
    Trap64 = 9,
    MemoryAccess32 = 10,
    MemoryAccess64 = 11,
    Io = 12,
    Memory = 13,
};

enum NodeFlag : int32_t
{
    NodeFlag_BasicInfo      = 1 << static_cast<int32_t>(NodeType::BasicInfo),
    NodeFlag_Pc32           = 1 << static_cast<int32_t>(NodeType::Pc32),
    NodeFlag_Pc64           = 1 << static_cast<int32_t>(NodeType::Pc64),
    NodeFlag_IntReg32       = 1 << static_cast<int32_t>(NodeType::IntReg32),
    NodeFlag_IntReg64       = 1 << static_cast<int32_t>(NodeType::IntReg64),
    NodeFlag_Csr32          = 1 << static_cast<int32_t>(NodeType::Csr32),
    NodeFlag_Csr64          = 1 << static_cast<int32_t>(NodeType::Csr64),
    NodeFlag_Trap32         = 1 << static_cast<int32_t>(NodeType::Trap32),
    NodeFlag_Trap64         = 1 << static_cast<int32_t>(NodeType::Trap64),
    NodeFlag_MemoryAccess32 = 1 << static_cast<int32_t>(NodeType::MemoryAccess32),
    NodeFlag_MemoryAccess64 = 1 << static_cast<int32_t>(NodeType::MemoryAccess64),
    NodeFlag_Io             = 1 << static_cast<int32_t>(NodeType::Io),
    NodeFlag_Memory         = 1 << static_cast<int32_t>(NodeType::Memory),
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
    int8_t reserved[4];
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

struct IntReg32Node
{
    int32_t regs[32];
};

struct IntReg64Node
{
    int64_t regs[32];
};

struct Csr32NodeHeader
{
    int64_t bodySize;
};

struct Csr64NodeHeader
{
    int64_t bodySize;
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
    int32_t virtualAddress;
    int32_t physicalAddress;
    int32_t value;
    MemoryAccessType memoryAccessType;
    MemoryAccessSize memoryAccessSize;
    int8_t reserved[2];
};

struct MemoryAccess64Node
{
    int64_t virtualAddress;
    int64_t physicalAddress;
    int64_t value;
    MemoryAccessType memoryAccessType;
    MemoryAccessSize memoryAccessSize;
    int8_t reserved[6];
};

struct IoNode
{
    int32_t hostIoValue;
    int32_t reserved;
};

struct MemoryNodeHeader
{
    int64_t bodySize;
};

}