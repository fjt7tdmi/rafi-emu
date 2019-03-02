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

#include <cstddef>
#include <cstdint>

namespace rafi {

const int IntRegCount = 32;
const int FpRegCount = 32;

enum class XLEN : uint32_t
{
    XLEN32 = 1,
    XLEN64 = 2,
    XLEN128 = 3,
};

enum class AddressTranslationMode : uint32_t
{
    Bare = 0,
    Sv32 = 1,
    Sv39 = 8,
    Sv48 = 9,
    Sv57 = 10,
    Sv64 = 11,
};

enum class MemoryAccessType : uint32_t
{
    Instruction = 0,
    Load = 2,
    Store = 3,
};

enum class PrivilegeLevel : uint32_t
{
    User = 0,
    Supervisor = 1,
    Reserved = 2,
    Machine = 3,
};

enum class TrapType : uint32_t
{
    Interrupt = 0,
    Exception = 1,
    Return = 2,
};

enum class InterruptType : uint32_t
{
    UserSoftware = 0,
    SupervisorSoftware = 1,
    MachineSoftware = 3,
    UserTimer = 4,
    SupervisorTimer = 5,
    MachineTimer = 7,
    UserExternal = 8,
    SupervisorExternal = 9,
    MachineExternal = 11,
};

enum class ExceptionType : uint32_t
{
    InstructionAddressMisaligned = 0,
    InstructionAccessFault = 1,
    IllegalInstruction = 2,
    Breakpoint = 3,
    LoadAddressMisaligned = 4,
    LoadAccessFault = 5,
    StoreAddressMisaligned = 6,
    StoreAccessFault = 7,
    EnvironmentCallFromUser = 8,
    EnvironmentCallFromSupervisor = 9,
    EnvironmentCallFromMachine = 11,
    InstructionPageFault = 12,
    LoadPageFault = 13,
    StorePageFault = 15,
};

}
