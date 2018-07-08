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

#include "../Common/BasicTypes.h"
#include "../Common/RvTypes.h"

const char TraceBinarySignature[8] = "RVTRACE";

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

struct TraceBinaryHeader
{
    char signature[8];
    int64_t headerSize;
};

struct TraceHeader
{
    int64_t nodeSize; // including TraceHeader itself
};

struct TraceChildHeader
{
    int64_t nodeSize; // including TraceChildHeader itself
    NodeType nodeType;
    int32_t reserved;
};

struct BasicInfoNode
{
    TraceChildHeader header;
    int32_t cycle;
    int32_t opId;
    int32_t insn;
    int8_t reserved[4];
};

struct Pc32Node
{
    TraceChildHeader header;
    int32_t virtualPc;
    int32_t physicalPc;
};

struct Pc64Node
{
    TraceChildHeader header;
    int64_t virtualPc;
    int64_t physicalPc;
};

struct IntReg32Node
{
    TraceChildHeader header;
    int32_t regs[32];
};

struct IntReg64Node
{
    TraceChildHeader header;
    int64_t regs[32];
};

struct Csr32NodeHeader
{
    TraceChildHeader header;
    int64_t bodySize;
};

struct Csr64NodeHeader
{
    TraceChildHeader header;
    int64_t bodySize;
};

struct Trap32Node
{
    TraceChildHeader header;
    TrapType trapType;
    PrivilegeLevel from;
    PrivilegeLevel to;
    int32_t cause;
    int32_t trapValue;
    int32_t reserved;
};

struct Trap64Node
{
    TraceChildHeader header;
    TrapType trapType;
    PrivilegeLevel from;
    PrivilegeLevel to;
    int8_t cause;
    int8_t reserved[4];
    int64_t trapValue;
};

struct MemoryAccess32Node
{
    TraceChildHeader header;
    int32_t virtualAddress;
    int32_t physicalAddress;
    int32_t value;
    MemoryAccessType memoryAccessType;
    MemoryAccessSize memoryAccessSize;
    int8_t reserved[2];
};

struct MemoryAccess64Node
{
    TraceChildHeader header;
    int64_t virtualAddress;
    int64_t physicalAddress;
    int64_t value;
    MemoryAccessType memoryAccessType;
    MemoryAccessSize memoryAccessSize;
    int8_t reserved[6];
};

struct IoNode
{
    TraceChildHeader header;
    int32_t hostIoValue;
    int32_t reserved;
};

struct MemoryNodeHeader
{
    TraceChildHeader header;
    int64_t bodySize;
};
