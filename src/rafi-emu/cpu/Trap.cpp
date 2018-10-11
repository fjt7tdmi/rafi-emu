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

#include <cstring>

#include "Trap.h"

Trap MakeInstructionAddressMisalignedException(int32_t pc)
{
    return Trap { ExceptionType::InstructionAddressMisaligned, pc, pc };
}

Trap MakeInstructionAccessFaultException(int32_t pc)
{
    return Trap { ExceptionType::InstructionAccessFault, pc, pc };
}

Trap MakeIllegalInstructionException(int32_t pc, int32_t insn)
{
    return Trap { ExceptionType::IllegalInstruction, pc, insn };
}

Trap MakeBreakpointException(int32_t pc)
{
    return Trap { ExceptionType::Breakpoint, pc, pc };
}

Trap MakeLoadAddressMisalignedException(int32_t pc, int32_t address)
{
    return Trap { ExceptionType::LoadAddressMisaligned, pc, address };
}

Trap MakeLoadAccessFaultException(int32_t pc, int32_t address)
{
    return Trap { ExceptionType::LoadAccessFault, pc, address };
}

Trap MakeStoreAccessFaultException(int32_t pc, int32_t address)
{
    return Trap { ExceptionType::StoreAccessFault, pc, address };
}

Trap MakeEnvironmentCallFromUserException(int32_t pc)
{
    return Trap { ExceptionType::EnvironmentCallFromUser, pc, pc };
}

Trap MakeEnvironmentCallFromSupervisorException(int32_t pc)
{
    return Trap { ExceptionType::EnvironmentCallFromSupervisor, pc, pc };
}

Trap MakeEnvironmentCallFromMachineException(int32_t pc)
{
    return Trap { ExceptionType::EnvironmentCallFromMachine, pc, pc };
}

Trap MakeInstructionPageFaultException(int32_t pc)
{
    return Trap { ExceptionType::InstructionPageFault, pc, pc };
}

Trap MakeLoadPageFaultException(int32_t pc, int32_t address)
{
    return Trap { ExceptionType::LoadPageFault, pc, address };
}

Trap MakeStorePageFaultException(int32_t pc, int32_t address)
{
    return Trap { ExceptionType::StorePageFault, pc, address };
}
