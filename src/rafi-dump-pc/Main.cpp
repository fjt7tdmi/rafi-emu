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

#include <rafi/trace.h>

using namespace rafi::trace;

namespace po = boost::program_options;

namespace {

void PrintTrace(const std::string& path, int startCycle, int count, bool showVirtual)
{
    FileTraceReader reader(path.c_str());

    for (int i = 0; i < startCycle + count; i++)
    {
        if (reader.IsEnd())
        {
            return;
        }

        if (i >= startCycle)
        {
            TraceCycleReader cycle(reader.GetCurrentCycleData(), reader.GetCurrentCycleDataSize());

            if (cycle.GetNodeCount(NodeType::Pc32) > 0)
            {
                const auto node = cycle.GetPc32Node();
                printf("0x%08x\n", showVirtual ? node->virtualPc : node->physicalPc);
            }
            else
            {
                printf("Error: no pc data. (cycle = %d)\n", i);
                exit(1);
            }
        }

        reader.MoveToNextCycle();
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
        ("virtual", "show virtual address (default is physical address)")
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

    const bool showVirtual = optMap.count("virtual") != 0;

    PrintTrace(optMap["input-file"].as<std::string>(), startCycle, count, showVirtual);
    return 0;
}
