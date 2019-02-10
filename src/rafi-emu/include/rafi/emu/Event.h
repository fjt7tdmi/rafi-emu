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

using namespace rafi::common;
using namespace rvtrace;

namespace rafi { namespace emu {

struct CsrReadEvent
{
    csr_addr_t address;
    uint32_t value;
};

struct CsrWriteEvent
{
    csr_addr_t address;
    uint32_t value;
};

struct MemoryAccessEvent
{
    MemoryAccessType accessType;
    uint32_t size;
    uint64_t value;
    uint64_t virtualAddress;
    PhysicalAddress physicalAddress;
};

struct OpEvent
{
    uint32_t opId;
    uint32_t virtualPc;
    PhysicalAddress physicalPc;
    uint32_t insn;
    OpCode opCode;
    PrivilegeLevel privilegeLevel;
};

struct TrapEvent
{
    TrapType trapType;
    uint32_t trapCause;
    PrivilegeLevel from;
    PrivilegeLevel to;
    uint32_t trapValue;
};

}}
