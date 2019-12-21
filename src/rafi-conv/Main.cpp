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

#include <iostream>
#include <memory>

#include <rafi/trace.h>

#include "../util/TraceUtil.h"

namespace rafi {

void Convert(const char* inPath, const char* outPathBase)
{
    try
    {
        auto reader = MakeTraceReader(inPath);
        auto writer = std::make_unique<trace::TraceIndexWriter>(outPathBase);

        while (!reader->IsEnd())
        {
            const auto cycle = reader->GetCycle()->GetCycle();
            const auto xlen = reader->GetCycle()->GetXLEN();
            const auto pc = reader->GetCycle()->GetPc();

            trace::BinaryCycleLogger cycleLogger(cycle, xlen, pc);

            if (reader->GetCycle()->IsIntRegExist())
            {
                if (xlen == XLEN::XLEN32)
                {
                    trace::NodeIntReg32 node;
                    for (int i = 0; i < 32; i++)
                    {
                        node.regs[i] = static_cast<uint32_t>(reader->GetCycle()->GetIntReg(i));
                    }
                    cycleLogger.Add(node);
                }
                else if (xlen == XLEN::XLEN64)
                {
                    trace::NodeIntReg64 node;
                    for (int i = 0; i < 32; i++)
                    {
                        node.regs[i] = reader->GetCycle()->GetIntReg(i);
                    }
                    cycleLogger.Add(node);
                }
                else
                {
                    RAFI_NOT_IMPLEMENTED;
                }
            }

            cycleLogger.Break();

            writer->Write(cycleLogger.GetData(), static_cast<int64_t>(cycleLogger.GetDataSize()));

            reader->Next();
        }
    }
    catch (const FileOpenFailureException& e)
    {
        e.PrintMessage();
    }
    catch (const trace::TraceException& e)
    {
        e.PrintMessage();
    }
}

}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "usage: " << argv[0] << "<in path> <out path base>" << std::endl;
    }

    rafi::Convert(argv[1], argv[2]);

    return 0;
}
