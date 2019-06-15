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
