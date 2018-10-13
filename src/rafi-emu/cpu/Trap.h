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

#include <rvtrace/common.h>

using namespace rvtrace;

namespace rafi { namespace cpu {

struct Trap
{
    ExceptionType type;
    int32_t pc;
    int32_t trapValue;
};

Trap MakeInstructionAddressMisalignedException(int32_t pc);
Trap MakeInstructionAccessFaultException(int32_t pc);
Trap MakeIllegalInstructionException(int32_t pc, int32_t insn);
Trap MakeBreakpointException(int32_t pc);
Trap MakeLoadAddressMisalignedException(int32_t pc, int32_t address);
Trap MakeLoadAccessFaultException(int32_t pc, int32_t address);
Trap MakeStoreAccessFaultException(int32_t pc, int32_t address);
Trap MakeEnvironmentCallFromUserException(int32_t pc);
Trap MakeEnvironmentCallFromSupervisorException(int32_t pc);
Trap MakeEnvironmentCallFromMachineException(int32_t pc);
Trap MakeInstructionPageFaultException(int32_t pc);
Trap MakeLoadPageFaultException(int32_t pc, int32_t address);
Trap MakeStorePageFaultException(int32_t pc, int32_t address);

}}
