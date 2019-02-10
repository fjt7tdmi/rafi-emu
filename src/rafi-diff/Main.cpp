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

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <rafi/trace.h>

#include "CycleComparator.h"

using namespace std;
using namespace rafi::common;
using namespace rafi::trace;

namespace po = boost::program_options;

namespace {

const char* Pass = "[  PASS  ]";
const char* Failed = "[ FAILED ]";

void CompareTrace(const string& expectPath, const string& actualPath, bool cmpPhysicalPc, bool cmpCsr, bool cmpMemory)
{
    CycleComparator comparator(cmpPhysicalPc, cmpCsr, cmpMemory);

    FileTraceReader expectReader(expectPath.c_str());
    FileTraceReader actualReader(actualPath.c_str());

    int expectOpCount = 0;
    int actualOpCount = 0;

    while (!expectReader.IsEnd() && !actualReader.IsEnd())
    {
        TraceCycleReader expectCycle(expectReader.GetCurrentCycleData(), expectReader.GetCurrentCycleDataSize());
        TraceCycleReader actualCycle(actualReader.GetCurrentCycleData(), actualReader.GetCurrentCycleDataSize());

        if (!comparator.AreMatched(expectCycle, actualCycle))
        {
            std::cout << std::hex << "Archtecture state is not matched for opId 0x" << expectOpCount << "." << std::endl;
            comparator.PrintDiff(expectCycle, actualCycle);
            std::cout << Failed << std::endl;

            return;
        }

        expectReader.MoveToNextCycle();
        actualReader.MoveToNextCycle();

        expectOpCount++;
        actualOpCount++;
    }

    // Count ops
    while (!expectReader.IsEnd())
    {
        expectReader.MoveToNextCycle();
        expectOpCount++;
    }
    while (!actualReader.IsEnd())
    {
        actualReader.MoveToNextCycle();
        actualOpCount++;
    }

    std::cout << "All compared ops are matched. (" << actualOpCount << " ops compared)" << std::endl;
    std::cout << "    - expect trace has " << expectOpCount << " ops." << std::endl;
    std::cout << "    - actual trace has " << actualOpCount << " ops." << std::endl;
    std::cout << Pass << std::endl;
}

}

int main(int argc, char** argv)
{
    po::options_description optionDesc("options");
    optionDesc.add_options()
        ("cmp-physical-pc", "enable comparing physical PC")
        ("cmp-csr", "enable comparing physical PC")
        ("cmp-memory", "enable comparing physical PC")
        ("expect", po::value<string>()->required(), "expect trace binary")
        ("actual", po::value<string>()->required(), "actual trace binary")
        ("help", "show help");

    po::variables_map optionMap;
    try
    {
        po::store(po::parse_command_line(argc, argv, optionDesc), optionMap);
    }
    catch (const boost::program_options::error_with_option_name& e)
    {
        std::cout << e.what() << std::endl;
        exit(1);
    }

    try
    {
        po::notify(optionMap);
    }
    catch (const boost::program_options::required_option& e)
    {
        std::cout << e.what() << std::endl;
        exit(1);
    }

    if (optionMap.count("help"))
    {
        std::cout << optionDesc << std::endl;
        exit(0);
    }

    const bool cmpPhysicalPc = optionMap.count("cmp-physical-pc") != 0;
    const bool cmpCsr = optionMap.count("cmp-csr") != 0;
    const bool cmpMemory = optionMap.count("cmp-memory") != 0;

    try
    {
        CompareTrace(optionMap["expect"].as<string>(), optionMap["actual"].as<string>(), cmpPhysicalPc, cmpCsr, cmpMemory);
    }
    catch (const TraceException& e)
    {
        e.PrintMessage();
    }
    catch (const TraceCycleException& e)
    {
        std::cout << e.GetMessage() << std::endl;
    }

    return 0;
}
