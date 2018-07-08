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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include "Common/Exception.h"
#include "Trace/TraceBinaryReader.h"
#include "Trace/TraceBinaryUtil.h"

namespace po = boost::program_options;

namespace {

void PrintBasicInfoNode(const BasicInfoNode& node)
{
    printf(
        "  Basic {\n"
        "    cycle: 0x%08x\n"
        "    opId:  0x%08x\n"
        "    insn:  0x%08x\n"
        "  }\n",
        node.cycle,
        node.opId,
        node.insn
    );
}

void PrintIoNode(const IoNode& node)
{
    printf(
        "  Io {\n"
        "    host: 0x%08x\n"
        "  }\n",
        node.hostIoValue
    );
}

void PrintPc32Node(const Pc32Node& node)
{
    printf(
        "  Pc32 {\n"
        "    virtualPc:  0x%08x\n"
        "    physicalPc: 0x%08x\n"
        "  }\n",
        node.virtualPc,
        node.physicalPc
    );
}

void PrintIntReg32Node(const IntReg32Node& node)
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
        node.regs[0],
        node.regs[1],
        node.regs[2],
        node.regs[3],
        node.regs[4],
        node.regs[5],
        node.regs[6],
        node.regs[7],
        node.regs[8],
        node.regs[9],
        node.regs[10],
        node.regs[11],
        node.regs[12],
        node.regs[13],
        node.regs[14],
        node.regs[15],
        node.regs[16],
        node.regs[17],
        node.regs[18],
        node.regs[19],
        node.regs[20],
        node.regs[21],
        node.regs[22],
        node.regs[23],
        node.regs[24],
        node.regs[25],
        node.regs[26],
        node.regs[27],
        node.regs[28],
        node.regs[29],
        node.regs[30],
        node.regs[31]
    );
}

void PrintCsr32Node(const void* pBody, int64_t bodySize)
{
    if (bodySize % sizeof(int32_t) != 0)
    {
        throw TraceException("Body size of Csr32Node must be multiple of 4.");
    }

    const auto count = bodySize / sizeof(int32_t);
    const auto p = reinterpret_cast<const int32_t*>(pBody);

    printf("  Csr32 {\n");
    for (int64_t i = 0; i < count; i++)
    {
        printf("    0x%03I64x: 0x%08x // %s\n", i, p[i], GetString(static_cast<csr_addr_t>(i), ""));
    }
    printf("  }\n");
}

void PrintTrap32Node(const Trap32Node& node)
{
    printf(
        "  Trap32 {\n"
        "    type: %s\n"
        "    from: %s\n"
        "    to:   %s\n"
        "    cause:     0x%08x\n"
        "    trapValue: 0x%08x\n"
        "  }\n",
        GetString(node.trapType),
        GetString(node.from),
        GetString(node.to),
        node.cause,
        node.trapValue
    );
}

void PrintMemoryAccess32Node(const MemoryAccess32Node& node)
{
    printf(
        "  MemoryAccess32 {\n"
        "    vaddr: 0x%08x\n"
        "    paddr: 0x%08x\n"
        "    value: 0x%08x\n"
        "    accessType: %s\n"
        "    accessSize: %s\n"
        "  }\n",
        node.virtualAddress,
        node.physicalAddress,
        node.value,
        GetString(node.memoryAccessType),
        GetString(node.memoryAccessSize)
    );
}

// TODO: implement PrintMemoryNode()

void PrintTraceChilds(char* buffer, size_t nodeSize, int cycle)
{
    printf("{ // cycle: 0x%08x\n", cycle);

    if (auto p = FindBasicInfoNode(buffer, nodeSize); p != nullptr)
    {
        PrintBasicInfoNode(*p);
    }
    if (auto p = FindIoNode(buffer, nodeSize); p != nullptr)
    {
        PrintIoNode(*p);
    }
    if (auto p = FindPc32Node(buffer, nodeSize); p != nullptr)
    {
        PrintPc32Node(*p);
    }
    if (auto p = FindIntReg32Node(buffer, nodeSize); p != nullptr)
    {
        PrintIntReg32Node(*p);
    }
    if (auto p = FindCsr32Node(buffer, nodeSize); p != nullptr)
    {
        auto pBody = &p[1];
        PrintCsr32Node(pBody, p->bodySize);
    }
    if (auto p = FindTrap32Node(buffer, nodeSize); p != nullptr)
    {
        PrintTrap32Node(*p);
    }
    if (auto p = FindMemoryAccess32Node(buffer, nodeSize); p != nullptr)
    {
        PrintMemoryAccess32Node(*p);
    }

    printf("}\n");
}

void PrintTrace(const std::string& path, int startCycle, int count)
{
    TraceBinaryReader reader(path.c_str());
    reader.MoveToFirst();

    for (int i = 0; i < startCycle + count; i++)
    {
        if (reader.IsEndNode())
        {
            return;
        }

        if (i >= startCycle)
        {
            PrintTraceChilds(reader.GetNode(), reader.GetNodeSize(), i);
        }

        reader.MoveToNext();
    }
}

}

int main(int argc, char** argv)
{
    const int DefaultCount = 1000 * 1000 * 1000;

    int startCycle;
    int count;

    po::options_description optDesc("options");
    optDesc.add_options()
        ("count,c", po::value<int>(&count)->default_value(DefaultCount), "number of cycles to print")
        ("start-cycle,s", po::value<int>(&startCycle)->default_value(0), "cycle to start print")
        ("input-file", po::value<std::string>(), "input trace binary path")
        ("help", "show help");

    po::positional_options_description posOptDesc;
    posOptDesc.add("input-file", -1);

    po::variables_map optMap;
    try
    {
        po::store(po::command_line_parser(argc, argv).options(optDesc).positional(posOptDesc).run(), optMap);
    }
    catch (const boost::program_options::error_with_option_name& e)
    {
        std::cout << e.what() << std::endl;
        exit(1);
    }
    po::notify(optMap);

    if (optMap.count("help") > 0 || optMap.count("input-file") == 0)
    {
        std::cout << optDesc << std::endl;
        return 0;
    }

    PrintTrace(optMap["input-file"].as<std::string>(), startCycle, count);
    return 0;
}
