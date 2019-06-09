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

#include <rafi/trace.h>

#include "../util/TraceUtil.h"

#include "CommandLineOption.h"
#include "CycleComparator.h"

using namespace rafi::trace;

namespace rafi {

void CompareTrace(ITraceReader* expect, ITraceReader* actual, const CommandLineOption& option)
{
    const int StopComparationThreshold = option.GetThreshold();

    CycleComparator comparator(option.CheckPhysicalPc());

    int continuousUnmatchCount = 0;

    int checkOpCount = 0;
    int expectOpCount = 0;
    int actualOpCount = 0;

    for (int i = 0; i < option.GetCycleCount(); i++)
    {
        if (expect->IsEnd() || actual->IsEnd())
        {
            break;
        }

        const auto expectCycle = expect->GetCycle();
        const auto actualCycle = actual->GetCycle();

        if (comparator.IsMatched(expectCycle, actualCycle))
        {
            continuousUnmatchCount = 0;

            expect->Next();
            actual->Next();

            checkOpCount++;
            expectOpCount++;
            actualOpCount++;
        }
        else
        {
            std::string expectNote;
            if (expectCycle->IsNoteExist())
            {
                expectCycle->CopyNote(&expectNote);
            }

            std::string actualNote;
            if (actualCycle->IsNoteExist())
            {
                actualCycle->CopyNote(&actualNote);
            }

            std::cout << "Detect mismatched cycle." << std::endl;
            std::cout << "    - expect: 0x" << std::hex << expectOpCount << " (" << std::dec << expectOpCount << ") cycle. note: " << expectNote << std::endl;
            std::cout << "    - actual: 0x" << std::hex << actualOpCount << " (" << std::dec << actualOpCount << ") cycle. note: " << actualNote << std::endl;
            std::cout << "Proceed actual." << std::endl;

            comparator.PrintDiff(expectCycle, actualCycle);

            if (++continuousUnmatchCount == StopComparationThreshold)
            {
                std::cout << "==========================================" << std::endl;
                std::cout << "STOP: detect " << std::dec << StopComparationThreshold << " contiguous unmatced cycles" << std::endl;
                break;
            }

            actual->Next();
            actualOpCount++;
        }

        if (i > 0 && i % 100000 == 0)
        {
            std::cout << "Compare " << std::dec << i << " cycles." << std::endl;
        }
    }

    std::cout << "Comparation finished." << std::endl;
    std::cout << "    - expect: 0x" << std::hex << expectOpCount << " (" << std::dec << expectOpCount << ") ops." << std::endl;
    std::cout << "    - actual: 0x" << std::hex << actualOpCount << " (" << std::dec << actualOpCount << ") ops." << std::endl;
}

}

int main(int argc, char** argv)
{
    rafi::CommandLineOption option(argc, argv);

    auto expect = rafi::MakeTraceReader(option.GetExpectPath());
    auto actual = rafi::MakeTraceReader(option.GetActualPath());

    try
    {
        rafi::CompareTrace(expect.get(), actual.get(), option);
    }
    catch (const TraceException& e)
    {
        e.PrintMessage();
        return 1;
    }

    return 0;
}
