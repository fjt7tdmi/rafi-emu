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

    bool prevCycleMatched = false;

    while (!expect->IsEnd() && !actual->IsEnd())
    {
        const auto expectCycle = expect->GetCycle();
        const auto actualCycle = actual->GetCycle();

        if (comparator.IsMatched(expectCycle, actualCycle))
        {
            if (!prevCycleMatched)
            {
                std::cout << "Detect matched cycles." << std::endl;
                std::cout << "    - expect: " << expectOpCount << " cycle." << std::endl;
                std::cout << "    - actual: " << actualOpCount << " cycle." << std::endl;
                std::cout << "Proceed expect and actual." << std::endl;
            }
            prevCycleMatched = true;

            expect->Next();
            actual->Next();

            checkOpCount++;
            expectOpCount++;
            actualOpCount++;
        }
        else
        {
            if (prevCycleMatched)
            {
                std::cout << "Detect mismatched cycles." << std::endl;
                std::cout << "    - expect: " << expectOpCount << " cycle." << std::endl;
                std::cout << "    - actual: " << actualOpCount << " cycle." << std::endl;
                std::cout << "Proceed actual." << std::endl;
            }
            prevCycleMatched = false;

            actual->Next();
            actualOpCount++;
        }
    }

    std::cout << "Comparation finished." << std::endl;
    std::cout << "    - expect: " << expectOpCount << " ops." << std::endl;
    std::cout << "    - actual: " << actualOpCount << " ops." << std::endl;
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
