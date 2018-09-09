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
    int8_t reserved[4];
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
