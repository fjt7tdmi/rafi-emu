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
#include <string>
#include <vector>

#include <rafi/emu.h>

#include "bus/Bus.h"

#include "TraceLogger.h"

#include "CommandLineOption.h"
#include "System.h"

int main(int argc, char** argv)
{
    rafi::emu::CommandLineOption option(argc, argv);

    rafi::emu::System system(option.GetXLEN(), option.GetPc(), option.GetRamSize());
    rafi::emu::TraceLogger logger(option.GetXLEN(), option.GetTraceLoggerConfig(), &system);

    try
    {
        for (auto& loadOption: option.GetLoadOptions())
        {
            system.LoadFileToMemory(loadOption.GetPath().c_str(), loadOption.GetAddress());
        }
    }
    catch (rafi::FileOpenFailureException e)
    {
        e.PrintMessage();
        std::exit(1);
    }

    if (option.IsHostIoEnabled())
    {
        system.SetHostIoAddress(option.GetHostIoAddress());
    }

    system.SetDtbAddress(option.GetDtbAddress());

    int cycle = 0;

    try
    {
        for (; cycle < option.GetCycle(); cycle++)
        {
            const bool dumpEnabled = cycle >= option.GetDumpSkipCycle();
            const bool lastCycle = option.IsHostIoEnabled() && system.GetHostIoValue() != 0;

            if (dumpEnabled)
            {
                logger.BeginCycle(cycle, system.GetPc());
                logger.RecordState();
            }

            if (lastCycle)
            {
                if (dumpEnabled)
                {
                    logger.EndCycle();
                }
                break;
            }

            system.ProcessCycle();

            if (dumpEnabled)
            {
                logger.RecordEvent();
                logger.EndCycle();
            }
        }
    }
    catch (rafi::emu::RafiEmuException)
    {
        std::cout << "Emulation stopped by exception." << std::endl;
        system.PrintStatus();
    }

    std::cout << "Emulation finished @ cycle "
        << std::dec << cycle
        << std::hex << " (0x" << cycle << ")" << std::endl;

    return 0;
}

