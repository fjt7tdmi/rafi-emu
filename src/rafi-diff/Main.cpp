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

#include <boost/algorithm/string.hpp>

#include <rafi/trace.h>

#include "CommandLineOption.h"
#include "CycleComparator.h"

using namespace rafi::trace;

namespace rafi {

const char* Pass = "[  PASS  ]";
const char* Failed = "[ FAILED ]";

std::unique_ptr<ITraceReader> MakeTraceReader(const std::string& path)
{
    if (boost::algorithm::ends_with(path, ".tbin") ||
        boost::algorithm::ends_with(path, ".bin"))
    {
        return std::make_unique<FileTraceReader>(path.c_str());
    }
    else
    {
        return std::make_unique<TextTraceReader>(path.c_str());
    }
}

void CompareTrace(ITraceReader* expect, ITraceReader* actual, bool checkPhysicalPc)
{
    CycleComparator comparator(checkPhysicalPc);

    int checkOpCount = 0;
    int expectOpCount = 0;
    int actualOpCount = 0;

    while (!expect->IsEnd() && !actual->IsEnd())
    {
        const auto expectCycle = expect->GetCycle();
        const auto actualCycle = actual->GetCycle();

        if (!comparator.IsMatched(expectCycle, actualCycle))
        {
            std::cout << std::hex << "Archtecture state is not matched for opId 0x" << expectOpCount << "." << std::endl;
            comparator.PrintDiff(expectCycle, actualCycle);
            std::cout << Failed << std::endl;

            return;
        }

        expect->Next();
        actual->Next();

        checkOpCount++;
        expectOpCount++;
        actualOpCount++;
    }

    // Count ops
    while (!expect->IsEnd())
    {
        expect->Next();
        expectOpCount++;
    }
    while (!actual->IsEnd())
    {
        actual->Next();
        actualOpCount++;
    }

    std::cout << "All compared ops are matched. (" << checkOpCount << " ops compared)" << std::endl;
    std::cout << "    - expect trace has " << expectOpCount << " ops." << std::endl;
    std::cout << "    - actual trace has " << actualOpCount << " ops." << std::endl;
    std::cout << Pass << std::endl;
}

}

int main(int argc, char** argv)
{
    rafi::CommandLineOption option(argc, argv);
 
    auto expect = rafi::MakeTraceReader(option.GetExpectPath());
    auto actual = rafi::MakeTraceReader(option.GetActualPath());

    try
    {
        rafi::CompareTrace(expect.get(), actual.get(), option.CheckPhysicalPc());
    }
    catch (const TraceException& e)
    {
        e.PrintMessage();
        return 1;
    }

    return 0;
}
