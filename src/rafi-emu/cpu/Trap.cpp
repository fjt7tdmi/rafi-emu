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

namespace rafi { namespace emu { namespace cpu {

Trap MakeInstructionAddressMisalignedException(vaddr_t pc)
{
    return Trap { ExceptionType::InstructionAddressMisaligned, pc, pc };
}

Trap MakeInstructionAccessFaultException(vaddr_t pc)
{
    return Trap { ExceptionType::InstructionAccessFault, pc, pc };
}

Trap MakeIllegalInstructionException(vaddr_t pc, uint32_t insn)
{
    return Trap { ExceptionType::IllegalInstruction, pc, insn };
}

Trap MakeBreakpointException(vaddr_t pc)
{
    return Trap { ExceptionType::Breakpoint, pc, pc };
}

Trap MakeLoadAddressMisalignedException(vaddr_t pc, vaddr_t address)
{
    return Trap { ExceptionType::LoadAddressMisaligned, pc, address };
}

Trap MakeLoadAccessFaultException(vaddr_t pc, vaddr_t address)
{
    return Trap { ExceptionType::LoadAccessFault, pc, address };
}

Trap MakeStoreAccessFaultException(vaddr_t pc, vaddr_t address)
{
    return Trap { ExceptionType::StoreAccessFault, pc, address };
}

Trap MakeEnvironmentCallFromUserException(vaddr_t pc)
{
    return Trap { ExceptionType::EnvironmentCallFromUser, pc, pc };
}

Trap MakeEnvironmentCallFromSupervisorException(vaddr_t pc)
{
    return Trap { ExceptionType::EnvironmentCallFromSupervisor, pc, pc };
}

Trap MakeEnvironmentCallFromMachineException(vaddr_t pc)
{
    return Trap { ExceptionType::EnvironmentCallFromMachine, pc, pc };
}

Trap MakeInstructionPageFaultException(vaddr_t pc)
{
    return Trap { ExceptionType::InstructionPageFault, pc, pc };
}

Trap MakeLoadPageFaultException(vaddr_t pc, vaddr_t address)
{
    return Trap { ExceptionType::LoadPageFault, pc, address };
}

Trap MakeStorePageFaultException(vaddr_t pc, vaddr_t address)
{
    return Trap { ExceptionType::StorePageFault, pc, address };
}

}}}
