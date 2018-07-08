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

#include "Common/Exception.h"
#include "Trace/TraceBinaryReader.h"
#include "Trace/TraceBinaryUtil.h"
#include "Trace/TraceBinaryComparator.h"

using namespace std;

namespace po = boost::program_options;

namespace {

const char* Pass = "[  PASS  ]";
const char* Failed = "[ FAILED ]";

void CompareTrace(const string& expectPath, const string& actualPath, bool cmpPhysicalPc, bool cmpCsr, bool cmpMemory)
{
    TraceBinaryComparator comparator(cmpPhysicalPc, cmpCsr, cmpMemory);

    TraceBinaryReader expectReader(expectPath.c_str());
    TraceBinaryReader actualReader(actualPath.c_str());

    int expectOpCount = 0;
    int actualOpCount = 0;

    while (!expectReader.IsEndNode() || !actualReader.IsEndNode())
    {
        if (!comparator.AreTraceChildsMatched(expectReader.GetNode(), expectReader.GetNodeSize(), actualReader.GetNode(), actualReader.GetNodeSize()))
        {
            std::cout << std::hex << "Archtecture state is not matched for opId 0x" << expectOpCount << "." << std::endl;
            comparator.PrintDiff(expectReader.GetNode(), expectReader.GetNodeSize(), actualReader.GetNode(), actualReader.GetNodeSize());
            std::cout << Failed << std::endl;

            return;
        }

        expectReader.MoveToNext();
        actualReader.MoveToNext();

        expectOpCount++;
        actualOpCount++;
    }

    // Count ops
    while (!expectReader.IsEndNode())
    {
        expectReader.MoveToNext();
        expectOpCount++;
    }
    while (!actualReader.IsEndNode())
    {
        actualReader.MoveToNext();
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

    CompareTrace(optionMap["expect"].as<string>(), optionMap["actual"].as<string>(), cmpPhysicalPc, cmpCsr, cmpMemory);

    return 0;
}
