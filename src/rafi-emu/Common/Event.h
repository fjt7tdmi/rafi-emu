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

#include <cstdint>

#include <rvtrace/common.h>

#include "BasicTypes.h"

using namespace rvtrace;

struct CsrReadEvent
{
    csr_addr_t address;
    int32_t value;
};

struct CsrWriteEvent
{
    csr_addr_t address;
    int32_t value;
};

struct MemoryAccessEvent
{
    MemoryAccessType accessType;
    MemoryAccessSize accessSize;
    int32_t virtualAddress;
    PhysicalAddress physicalAddress;
    int32_t value;
};

struct OpEvent
{
    int32_t opId;
    int32_t virtualPc;
    PhysicalAddress physicalPc;
    int32_t insn;
    OpCode opCode;
};

struct TrapEvent
{
    TrapType trapType;
    int32_t trapCause;
    PrivilegeLevel from;
    PrivilegeLevel to;
    int32_t trapValue;
};
