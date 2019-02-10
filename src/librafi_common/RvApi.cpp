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

#include <rafi/common.h>

namespace rafi {

namespace {
    const char* FpRegNames[32] =
    {
        "ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7",
        "fs0", "fs1",
        "fa0", "fa1", "fa2", "fa3", "fa4", "fa5", "fa6", "fa7",
        "fs2", "fs3", "fs4", "fs5", "fs6", "fs7", "fs8", "fs9", "fs10", "fs11",
        "ft8", "ft9", "ft10", "ft11",
    };
}

const char* GetString(MemoryAccessType accessType)
{
#define GET_STRING_CASE(arg) case MemoryAccessType::arg: return #arg
    switch (accessType)
    {
        GET_STRING_CASE(Instruction);
        GET_STRING_CASE(Load);
        GET_STRING_CASE(Store);
    default:
        return "Unknown MemoryAccessType";
    }
#undef GET_STRING_CASE
}

const char* GetString(PrivilegeLevel level)
{
#define GET_STRING_CASE(arg) case PrivilegeLevel::arg: return #arg
    switch (level)
    {
        GET_STRING_CASE(User);
        GET_STRING_CASE(Supervisor);
        GET_STRING_CASE(Reserved);
        GET_STRING_CASE(Machine);
    default:
        return "Unknown PrivilegeLevel";
    }
#undef GET_STRING_CASE
}

const char* GetString(TrapType trapType)
{
#define GET_STRING_CASE(arg) case TrapType::arg: return #arg
    switch (trapType)
    {
        GET_STRING_CASE(Interrupt);
        GET_STRING_CASE(Exception);
        GET_STRING_CASE(Return);
    default:
        return "Unknown TrapType";
    }
#undef GET_STRING_CASE
}

const char* GetString(ExceptionType exceptionType)
{
#define GET_STRING_CASE(arg) case ExceptionType::arg: return #arg
    switch (exceptionType)
    {
        GET_STRING_CASE(InstructionAddressMisaligned);
        GET_STRING_CASE(InstructionAccessFault);
        GET_STRING_CASE(IllegalInstruction);
        GET_STRING_CASE(Breakpoint);
        GET_STRING_CASE(LoadAddressMisaligned);
        GET_STRING_CASE(LoadAccessFault);
        GET_STRING_CASE(StoreAddressMisaligned);
        GET_STRING_CASE(StoreAccessFault);
        GET_STRING_CASE(EnvironmentCallFromUser);
        GET_STRING_CASE(EnvironmentCallFromSupervisor);
        GET_STRING_CASE(EnvironmentCallFromMachine);
        GET_STRING_CASE(InstructionPageFault);
        GET_STRING_CASE(LoadPageFault);
        GET_STRING_CASE(StorePageFault);
    default:
        return "Unknown PrivilegeLevel";
    }
#undef GET_STRING_CASE
}

const char* GetString(InterruptType interruptType)
{
#define GET_STRING_CASE(arg) case InterruptType::arg: return #arg
    switch (interruptType)
    {
        GET_STRING_CASE(UserSoftware);
        GET_STRING_CASE(SupervisorSoftware);
        GET_STRING_CASE(MachineSoftware);
        GET_STRING_CASE(UserTimer);
        GET_STRING_CASE(SupervisorTimer);
        GET_STRING_CASE(MachineTimer);
        GET_STRING_CASE(UserExternal);
        GET_STRING_CASE(SupervisorExternal);
        GET_STRING_CASE(MachineExternal);
    default:
        return "Unknown PrivilegeLevel";
    }
#undef GET_STRING_CASE
}

const char* GetString(csr_addr_t addr)
{
    return GetString(addr, "Unknown CSR address");
}

const char* GetString(csr_addr_t addr, const char* defaultValue)
{
    if (csr_addr_t::hpmcounter_begin <= addr && addr < csr_addr_t::hpmcounter_end)
    {
        return "hpmcounter*";
    }

    if (csr_addr_t::hpmcounterh_begin <= addr && addr < csr_addr_t::hpmcounter_end)
    {
        return "hpmcounterh*";
    }

    if (csr_addr_t::mhpmcounter_begin <= addr && addr < csr_addr_t::mhpmcounter_end)
    {
        return "mhpmcounter*";
    }

    if (csr_addr_t::mhpmcounterh_begin <= addr && addr < csr_addr_t::mhpmcounterh_end)
    {
        return "mhpmcounterh*";
    }

    if (csr_addr_t::pmpaddr_begin <= addr && addr < csr_addr_t::pmpaddr_end)
    {
        return "pmpaddr*";
    }

#define GET_STRING_CASE(arg) case csr_addr_t::arg: return #arg
    switch (addr)
    {
        GET_STRING_CASE(ustatus);
        GET_STRING_CASE(uie);
        GET_STRING_CASE(utvec);

        GET_STRING_CASE(uscratch);
        GET_STRING_CASE(uepc);
        GET_STRING_CASE(ucause);
        GET_STRING_CASE(utval);
        GET_STRING_CASE(uip);

        GET_STRING_CASE(fflags);
        GET_STRING_CASE(frm);
        GET_STRING_CASE(fcsr);

        GET_STRING_CASE(sstatus);
        GET_STRING_CASE(sedeleg);
        GET_STRING_CASE(sideleg);
        GET_STRING_CASE(sie);
        GET_STRING_CASE(stvec);
        GET_STRING_CASE(scounteren);

        GET_STRING_CASE(sscratch);
        GET_STRING_CASE(sepc);
        GET_STRING_CASE(scause);
        GET_STRING_CASE(stval);
        GET_STRING_CASE(sip);

        GET_STRING_CASE(satp);

        GET_STRING_CASE(mstatus);
        GET_STRING_CASE(misa);
        GET_STRING_CASE(medeleg);
        GET_STRING_CASE(mideleg);
        GET_STRING_CASE(mie);
        GET_STRING_CASE(mtvec);
        GET_STRING_CASE(mcounteren);

        GET_STRING_CASE(mscratch);
        GET_STRING_CASE(mepc);
        GET_STRING_CASE(mcause);
        GET_STRING_CASE(mtval);
        GET_STRING_CASE(mip);

        GET_STRING_CASE(pmpcfg0);
        GET_STRING_CASE(pmpcfg1);
        GET_STRING_CASE(pmpcfg2);
        GET_STRING_CASE(pmpcfg3);

        GET_STRING_CASE(tselect);
        GET_STRING_CASE(tdata1);
        GET_STRING_CASE(tdata2);
        GET_STRING_CASE(tdata3);

        GET_STRING_CASE(dcsr);
        GET_STRING_CASE(dpc);
        GET_STRING_CASE(dscratch);

        GET_STRING_CASE(mvendorid);
        GET_STRING_CASE(marchid);
        GET_STRING_CASE(mimpid);
        GET_STRING_CASE(mhartid);

    default:
        return defaultValue;
    }
#undef GET_STRING_CASE
}

const char* GetFpRegName(int index)
{
    if (!(0 <= index && index < 32))
    {
        return nullptr;
    }

    return FpRegNames[index];
}

}
