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

#include <rafi/trace.h>

#include "PrinterGdb.h"

namespace rafi {

PrinterGdb::PrinterGdb()
{
    std::memset(m_IntRegs, 0, sizeof(m_IntRegs));
}

void PrinterGdb::PrintCycle(const trace::CycleReader& cycle)
{
    const auto nodePc64 = cycle.GetPc64Node();
    const auto nodeIntReg64 = cycle.GetIntReg64Node();

    printf(
        "GDB [\n"
        "ra             0x%016llx\t%llu\n"
        "sp             0x%016llx\t%llu\n"
        "gp             0x%016llx\t%llu\n"
        "tp             0x%016llx\t%llu\n"
        "t0             0x%016llx\t%llu\n"
        "t1             0x%016llx\t%llu\n"
        "t2             0x%016llx\t%llu\n"
        "s0             0x%016llx\t%llu\n"
        "s1             0x%016llx\t%llu\n"
        "a0             0x%016llx\t%llu\n"
        "a1             0x%016llx\t%llu\n"
        "a2             0x%016llx\t%llu\n"
        "a3             0x%016llx\t%llu\n"
        "a4             0x%016llx\t%llu\n"
        "a5             0x%016llx\t%llu\n"
        "a6             0x%016llx\t%llu\n"
        "a7             0x%016llx\t%llu\n"
        "s2             0x%016llx\t%llu\n"
        "s3             0x%016llx\t%llu\n"
        "s4             0x%016llx\t%llu\n"
        "s5             0x%016llx\t%llu\n"
        "s6             0x%016llx\t%llu\n"
        "s7             0x%016llx\t%llu\n"
        "s8             0x%016llx\t%llu\n"
        "s9             0x%016llx\t%llu\n"
        "s10            0x%016llx\t%llu\n"
        "s11            0x%016llx\t%llu\n"
        "t3             0x%016llx\t%llu\n"
        "t4             0x%016llx\t%llu\n"
        "t5             0x%016llx\t%llu\n"
        "t6             0x%016llx\t%llu\n"
        "pc             0x%016llx\t%llu\n"
        "priv           [Invalid]\n"
        "]\n",
        m_IntRegs[1], m_IntRegs[1],
        m_IntRegs[2], m_IntRegs[2],
        m_IntRegs[3], m_IntRegs[3],
        m_IntRegs[4], m_IntRegs[4],
        m_IntRegs[5], m_IntRegs[5],
        m_IntRegs[6], m_IntRegs[6],
        m_IntRegs[7], m_IntRegs[7],
        m_IntRegs[8], m_IntRegs[8],
        m_IntRegs[9], m_IntRegs[9],
        m_IntRegs[10], m_IntRegs[10],
        m_IntRegs[11], m_IntRegs[11],
        m_IntRegs[12], m_IntRegs[12],
        m_IntRegs[13], m_IntRegs[13],
        m_IntRegs[14], m_IntRegs[14],
        m_IntRegs[15], m_IntRegs[15],
        m_IntRegs[16], m_IntRegs[16],
        m_IntRegs[17], m_IntRegs[17],
        m_IntRegs[18], m_IntRegs[18],
        m_IntRegs[19], m_IntRegs[19],
        m_IntRegs[20], m_IntRegs[20],
        m_IntRegs[21], m_IntRegs[21],
        m_IntRegs[22], m_IntRegs[22],
        m_IntRegs[23], m_IntRegs[23],
        m_IntRegs[24], m_IntRegs[24],
        m_IntRegs[25], m_IntRegs[25],
        m_IntRegs[26], m_IntRegs[26],
        m_IntRegs[27], m_IntRegs[27],
        m_IntRegs[28], m_IntRegs[28],
        m_IntRegs[29], m_IntRegs[29],
        m_IntRegs[30], m_IntRegs[30],
        m_IntRegs[31], m_IntRegs[31],
        static_cast<unsigned long long>(nodePc64->virtualPc), static_cast<unsigned long long>(nodePc64->virtualPc)
    );

    SaveIntRegs(nodeIntReg64);
}

void PrinterGdb::SaveIntRegs(const trace::IntReg64Node* pNode)
{
    for (int i = 0; i < 32; i++)
    {
        m_IntRegs[i] = static_cast<unsigned long long>(pNode->regs[i]);
    }
}

}
