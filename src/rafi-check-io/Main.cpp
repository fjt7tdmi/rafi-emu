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

namespace rafi {

namespace {
    const char* Pass = "[  PASS  ]";
    const char* Failed = "[ FAILED ]";
    uint32_t ExpectedHostIoValue = 1;
}

uint32_t GetLastHostIoValue(trace::ITraceReader* pReader)
{
    uint32_t hostIoValue = 0;

    while (!pReader->IsEnd())
    {
        trace::NodeIo io;

        pReader->GetCycle()->CopyIo(&io);
        pReader->Next();

        hostIoValue = io.hostIo;
    }

    return hostIoValue;
}

bool CheckIo(const char* name, const char* path)
{
    try
    {
        auto reader = MakeTraceReader(path);

        // Find IoNode
        const auto hostIoValue = GetLastHostIoValue(reader.get());

        // Check IoValue
        if (hostIoValue != ExpectedHostIoValue)
        {
            std::cout << Failed << " " << name << " ("
                << std::hex << "hostIoValue:0x" << hostIoValue << " "
                << std::dec << "testId:" << (hostIoValue / 2)
                << ")" << std::endl;
            return false;
        }

        std::cout << Pass << " " << name << std::endl;
    }
    catch (const FileOpenFailureException& e)
    {
        e.PrintMessage();
        return false;
    }
    catch (const trace::TraceException& e)
    {
        e.PrintMessage();
        return false;
    }

    return true;
}

}

int main(int argc, char** argv)
{
    if (argc < 1)
    {
        std::cout << "usage: " << argv[0] << "[<trace binary path> ...]" << std::endl;
    }

    int passCount = 0;
    int failCount = 0;

    for (int i = 1; i < argc; i++)
    {
        if (rafi::CheckIo(argv[i], argv[i]))
        {
            passCount++;
        }
        else
        {
            failCount++;
        }
    }

    const int testCount = passCount + failCount;

    std::cout << std::endl;
    std::cout << testCount << " tests executed (" << passCount << " passed, " << failCount << " failed)." << std::endl;

    return failCount;
}
