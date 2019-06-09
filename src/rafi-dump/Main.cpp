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

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <rafi/trace.h>

#include "../util/TraceUtil.h"

#include "CommandLineOption.h"
#include "CycleFilter.h"
#include "TraceTextPrinter.h"

namespace rafi { namespace dump {

const char* GetCauseString(TrapType trapType, uint32_t cause)
{
    switch (trapType)
    {
    case TrapType::Exception:
        return GetString(static_cast<ExceptionType>(cause));
    case TrapType::Interrupt:
        return GetString(static_cast<InterruptType>(cause));
    default:
        return "";
    }
}

void PrintBasicInfoNode(const trace::BasicInfoNode* node)
{
    Decoder decoder(XLEN::XLEN32);
    auto op = decoder.Decode(node->insn);

    char opStr[64];
    SNPrintOp(opStr, sizeof(opStr), op);

    printf(
        "  Basic {\n"
        "    cycle: 0x%08x\n"
        "    opId:  0x%08x\n"
        "    insn:  0x%08x // %s\n"
        "    priv:  %s\n"
        "  }\n",
        node->cycle,
        node->opId,
        node->insn,
        opStr,
        GetString(node->privilegeLevel)
    );
}

void PrintFpRegNode(const trace::FpRegNode* node)
{
    printf("  FpReg: {\n");

    for (int i = 0; i < 32; i++)
    {
        printf(
            "    f%-2d: { u64: 0x%016llx, f32: %e, f64: %e } // %s\n",
            i,
            static_cast<unsigned long long>(node->regs[i].u64.value),
            node->regs[i].f32.value,
            node->regs[i].f64.value,
            GetFpRegName(i)
        );
    }

    printf("  }\n");
}

void PrintIntReg32Node(const trace::IntReg32Node* node)
{
    printf(
        "  IntReg32: {\n"
        "    x0:  0x%08x // zero\n"
        "    x1:  0x%08x // ra\n"
        "    x2:  0x%08x // sp\n"
        "    x3:  0x%08x // gp\n"
        "    x4:  0x%08x // tp\n"
        "    x5:  0x%08x // t0\n"
        "    x6:  0x%08x // t1\n"
        "    x7:  0x%08x // t2\n"
        "    x8:  0x%08x // s0 (fp)\n"
        "    x9:  0x%08x // s1\n"
        "    x10: 0x%08x // a0\n"
        "    x11: 0x%08x // a1\n"
        "    x12: 0x%08x // a2\n"
        "    x13: 0x%08x // a3\n"
        "    x14: 0x%08x // a4\n"
        "    x15: 0x%08x // a5\n"
        "    x16: 0x%08x // a6\n"
        "    x17: 0x%08x // a7\n"
        "    x18: 0x%08x // s2\n"
        "    x19: 0x%08x // s3\n"
        "    x20: 0x%08x // s4\n"
        "    x21: 0x%08x // s5\n"
        "    x22: 0x%08x // s6\n"
        "    x23: 0x%08x // s7\n"
        "    x24: 0x%08x // s8\n"
        "    x25: 0x%08x // s9\n"
        "    x26: 0x%08x // s10\n"
        "    x27: 0x%08x // s11\n"
        "    x28: 0x%08x // t3\n"
        "    x29: 0x%08x // t4\n"
        "    x30: 0x%08x // t5\n"
        "    x31: 0x%08x // t6\n"
        "  }\n",
        node->regs[0],
        node->regs[1],
        node->regs[2],
        node->regs[3],
        node->regs[4],
        node->regs[5],
        node->regs[6],
        node->regs[7],
        node->regs[8],
        node->regs[9],
        node->regs[10],
        node->regs[11],
        node->regs[12],
        node->regs[13],
        node->regs[14],
        node->regs[15],
        node->regs[16],
        node->regs[17],
        node->regs[18],
        node->regs[19],
        node->regs[20],
        node->regs[21],
        node->regs[22],
        node->regs[23],
        node->regs[24],
        node->regs[25],
        node->regs[26],
        node->regs[27],
        node->regs[28],
        node->regs[29],
        node->regs[30],
        node->regs[31]
    );
}

void PrintIntReg64Node(const trace::IntReg64Node* node)
{
    printf(
        "  IntReg64: {\n"
        "    x0:  0x%016llx // zero\n"
        "    x1:  0x%016llx // ra\n"
        "    x2:  0x%016llx // sp\n"
        "    x3:  0x%016llx // gp\n"
        "    x4:  0x%016llx // tp\n"
        "    x5:  0x%016llx // t0\n"
        "    x6:  0x%016llx // t1\n"
        "    x7:  0x%016llx // t2\n"
        "    x8:  0x%016llx // s0 (fp)\n"
        "    x9:  0x%016llx // s1\n"
        "    x10: 0x%016llx // a0\n"
        "    x11: 0x%016llx // a1\n"
        "    x12: 0x%016llx // a2\n"
        "    x13: 0x%016llx // a3\n"
        "    x14: 0x%016llx // a4\n"
        "    x15: 0x%016llx // a5\n"
        "    x16: 0x%016llx // a6\n"
        "    x17: 0x%016llx // a7\n"
        "    x18: 0x%016llx // s2\n"
        "    x19: 0x%016llx // s3\n"
        "    x20: 0x%016llx // s4\n"
        "    x21: 0x%016llx // s5\n"
        "    x22: 0x%016llx // s6\n"
        "    x23: 0x%016llx // s7\n"
        "    x24: 0x%016llx // s8\n"
        "    x25: 0x%016llx // s9\n"
        "    x26: 0x%016llx // s10\n"
        "    x27: 0x%016llx // s11\n"
        "    x28: 0x%016llx // t3\n"
        "    x29: 0x%016llx // t4\n"
        "    x30: 0x%016llx // t5\n"
        "    x31: 0x%016llx // t6\n"
        "  }\n",
        static_cast<unsigned long long>(node->regs[0]),
        static_cast<unsigned long long>(node->regs[1]),
        static_cast<unsigned long long>(node->regs[2]),
        static_cast<unsigned long long>(node->regs[3]),
        static_cast<unsigned long long>(node->regs[4]),
        static_cast<unsigned long long>(node->regs[5]),
        static_cast<unsigned long long>(node->regs[6]),
        static_cast<unsigned long long>(node->regs[7]),
        static_cast<unsigned long long>(node->regs[8]),
        static_cast<unsigned long long>(node->regs[9]),
        static_cast<unsigned long long>(node->regs[10]),
        static_cast<unsigned long long>(node->regs[11]),
        static_cast<unsigned long long>(node->regs[12]),
        static_cast<unsigned long long>(node->regs[13]),
        static_cast<unsigned long long>(node->regs[14]),
        static_cast<unsigned long long>(node->regs[15]),
        static_cast<unsigned long long>(node->regs[16]),
        static_cast<unsigned long long>(node->regs[17]),
        static_cast<unsigned long long>(node->regs[18]),
        static_cast<unsigned long long>(node->regs[19]),
        static_cast<unsigned long long>(node->regs[20]),
        static_cast<unsigned long long>(node->regs[21]),
        static_cast<unsigned long long>(node->regs[22]),
        static_cast<unsigned long long>(node->regs[23]),
        static_cast<unsigned long long>(node->regs[24]),
        static_cast<unsigned long long>(node->regs[25]),
        static_cast<unsigned long long>(node->regs[26]),
        static_cast<unsigned long long>(node->regs[27]),
        static_cast<unsigned long long>(node->regs[28]),
        static_cast<unsigned long long>(node->regs[29]),
        static_cast<unsigned long long>(node->regs[30]),
        static_cast<unsigned long long>(node->regs[31])
    );
}

void PrintPc32Node(const trace::Pc32Node* node)
{
    printf(
        "  Pc32 {\n"
        "    vaddr: 0x%08x\n"
        "    paddr: 0x%08x\n"
        "  }\n",
        node->virtualPc,
        node->physicalPc
    );
}

void PrintPc64Node(const trace::Pc64Node* node)
{
    printf(
        "  Pc64 {\n"
        "    vaddr: 0x%016llx\n"
        "    paddr: 0x%016llx\n"
        "  }\n",
        static_cast<unsigned long long>(node->virtualPc),
        static_cast<unsigned long long>(node->physicalPc)
    );
}

void PrintTrap32Node(const trace::Trap32Node* node)
{
    printf(
        "  Trap32 {\n"
        "    type:  %s\n"
        "    from:  %s\n"
        "    to:    %s\n"
        "    cause: %s\n"
        "    trapValue: 0x%08x\n"
        "  }\n",
        GetString(node->trapType),
        GetString(node->from),
        GetString(node->to),
        GetCauseString(node->trapType, node->cause),
        node->trapValue
    );
}

void PrintTrap64Node(const trace::Trap64Node* node)
{
    printf(
        "  Trap64 {\n"
        "    type:  %s\n"
        "    from:  %s\n"
        "    to:    %s\n"
        "    cause: %s\n"
        "    trapValue: 0x%016llx\n"
        "  }\n",
        GetString(node->trapType),
        GetString(node->from),
        GetString(node->to),
        GetCauseString(node->trapType, node->cause),
        static_cast<unsigned long long>(node->trapValue)
    );
}

void PrintMemoryAccessNode(const trace::MemoryAccessNode* node)
{
    printf(
        "  MemoryAccess {\n"
        "    accessType: %s\n"
        "    size: %d // byte\n"
        "    value: 0x%016llx\n"
        "    vaddr: 0x%016llx\n"
        "    paddr: 0x%016llx\n"
        "  }\n",
        GetString(node->accessType),
        node->size,
        static_cast<unsigned long long>(node->value),
        static_cast<unsigned long long>(node->virtualAddress),
        static_cast<unsigned long long>(node->physicalAddress)
    );
}

void PrintCsr32Node(const trace::Csr32Node* pNodes, int64_t nodeCount)
{
    printf("  Csr32 {\n");

    for (int64_t i = 0; i < nodeCount; i++)
    {
        const auto address = static_cast<csr_addr_t>(pNodes[i].address);
        printf("%16s: 0x%08x\n", GetString(address), pNodes[i].value);
    }

    printf("  }\n");
}

void PrintCsr64Node(const trace::Csr64Node* pNodes, int64_t nodeCount)
{
    printf("  Cs64 {\n");

    for (int64_t i = 0; i < nodeCount; i++)
    {
        const auto address = static_cast<csr_addr_t>(pNodes[i].address);
        printf("%16s: 0x%016llx\n", GetString(address), static_cast<unsigned long long>(pNodes[i].value));
    }

    printf("  }\n");
}

void PrintIoNode(const trace::IoNode* node)
{
    printf(
        "  Io {\n"
        "    host: 0x%08x\n"
        "  }\n",
        node->hostIoValue
    );
}

void PrintCycle(const trace::CycleView& cycle, int cycleNum)
{
    printf("{ // cycle: 0x%08x\n", cycleNum);

    if (cycle.GetNodeCount(trace::NodeType::BasicInfo) > 0)
    {
        PrintBasicInfoNode(cycle.GetBasicInfoNode());
    }
    if (cycle.GetNodeCount(trace::NodeType::Io) > 0)
    {
        PrintIoNode(cycle.GetIoNode());
    }
    if (cycle.GetNodeCount(trace::NodeType::Pc32) > 0)
    {
        PrintPc32Node(cycle.GetPc32Node());
    }
    if (cycle.GetNodeCount(trace::NodeType::Pc64) > 0)
    {
        PrintPc64Node(cycle.GetPc64Node());
    }
    if (cycle.GetNodeCount(trace::NodeType::IntReg32) > 0)
    {
        PrintIntReg32Node(cycle.GetIntReg32Node());
    }
    if (cycle.GetNodeCount(trace::NodeType::IntReg64) > 0)
    {
        PrintIntReg64Node(cycle.GetIntReg64Node());
    }
    if (cycle.GetNodeCount(trace::NodeType::FpReg) > 0)
    {
        PrintFpRegNode(cycle.GetFpRegNode());
    }
    if (cycle.GetNodeCount(trace::NodeType::Trap32) > 0)
    {
        PrintTrap32Node(cycle.GetTrap32Node());
    }
    if (cycle.GetNodeCount(trace::NodeType::Trap64) > 0)
    {
        PrintTrap64Node(cycle.GetTrap64Node());
    }

    for (int index = 0; index < cycle.GetNodeCount(trace::NodeType::MemoryAccess); index++)
    {
        PrintMemoryAccessNode(cycle.GetMemoryAccessNode(index));
    }

    // TODO: implement PrintMemory()

    printf("}\n");
}

void PrintTrace(const CommandLineOption& option, IFilter* filter)
{
    auto reader = rafi::MakeTraceReader(option.GetPath());

    TraceTextPrinter m_TraceTextPrinter;

    const int begin = option.GetCycleBegin();
    const int end = std::min(option.GetCycleBegin() + option.GetCycleCount(), option.GetCycleEnd());

    for (int i = 0; i < end; i++)
    {
        if (reader->IsEnd())
        {
            return;
        }

        if (i >= begin && filter->Apply(reader->GetCycle()))
        {
            if (option.GetMode() == Mode::TraceText)
            {
                m_TraceTextPrinter.PrintCycle(reader->GetCycle());
            }
            else
            {
                //PrintCycle(reader->GetCycleView(), i);
                RAFI_NOT_IMPLEMENTED();
            }
        }

        reader->Next();
    }
}

}}

int main(int argc, char** argv)
{
    rafi::dump::CommandLineOption option(argc, argv);

    auto filter = rafi::dump::MakeFilter(option.GetFilterDescription());

    PrintTrace(option, filter.get());

    return 0;
}
