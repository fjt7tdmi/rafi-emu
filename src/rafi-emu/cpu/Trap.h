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
#include <variant>

#include <rafi/common.h>

using namespace rafi::common;

namespace rafi { namespace emu { namespace cpu {

struct Trap
{
    ExceptionType type;
    uint32_t pc;
    uint32_t trapValue;
};

Trap MakeInstructionAddressMisalignedException(uint32_t pc);
Trap MakeInstructionAccessFaultException(uint32_t pc);
Trap MakeIllegalInstructionException(uint32_t pc, uint32_t insn);
Trap MakeBreakpointException(uint32_t pc);
Trap MakeLoadAddressMisalignedException(uint32_t pc, uint32_t address);
Trap MakeLoadAccessFaultException(uint32_t pc, uint32_t address);
Trap MakeStoreAccessFaultException(uint32_t pc, uint32_t address);
Trap MakeEnvironmentCallFromUserException(uint32_t pc);
Trap MakeEnvironmentCallFromSupervisorException(uint32_t pc);
Trap MakeEnvironmentCallFromMachineException(uint32_t pc);
Trap MakeInstructionPageFaultException(uint32_t pc);
Trap MakeLoadPageFaultException(uint32_t pc, uint32_t address);
Trap MakeStorePageFaultException(uint32_t pc, uint32_t address);

}}}
