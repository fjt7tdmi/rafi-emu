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

#include <rafi/trace.h>

#include "BasicTypes.h"

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
    paddr_t physicalAddress;
};

struct OpEvent
{
    uint32_t opId;
    uint32_t insn;
    PrivilegeLevel privilegeLevel;
    vaddr_t virtualPc;
    paddr_t physicalPc;
};

struct TrapEvent
{
    TrapType trapType;
    uint32_t trapCause;
    PrivilegeLevel from;
    PrivilegeLevel to;
    uint64_t trapValue;
};

}}
