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
#include <cstdio>
#include <string>
#include <iostream>
#include <optional>
#include <variant>

#include <rafi/emu.h>

// _trap: std::optional<Trap>
#define RAFI_RETURN_IF_TRAP(_trap) \
    do \
    { \
        const auto _t = _trap; \
        if (_t) \
        { \
            return _t; \
        } \
    } while(0)

namespace rafi { namespace emu { namespace cpu {

struct Trap
{
    ExceptionType type;
    vaddr_t pc;
    uint64_t trapValue;
};

Trap MakeInstructionAddressMisalignedException(vaddr_t pc);
Trap MakeInstructionAccessFaultException(vaddr_t pc);
Trap MakeIllegalInstructionException(vaddr_t pc, uint32_t insn);
Trap MakeBreakpointException(vaddr_t pc);
Trap MakeLoadAddressMisalignedException(vaddr_t pc, vaddr_t address);
Trap MakeLoadAccessFaultException(vaddr_t pc, vaddr_t address);
Trap MakeStoreAccessFaultException(vaddr_t pc, vaddr_t address);
Trap MakeEnvironmentCallFromUserException(vaddr_t pc);
Trap MakeEnvironmentCallFromSupervisorException(vaddr_t pc);
Trap MakeEnvironmentCallFromMachineException(vaddr_t pc);
Trap MakeInstructionPageFaultException(vaddr_t pc, vaddr_t address);
Trap MakeLoadPageFaultException(vaddr_t pc, vaddr_t address);
Trap MakeStorePageFaultException(vaddr_t pc, vaddr_t address);

}}}
