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

#include "TraceTextLogger.h"
#include "Profiler.h"
#include "TraceLogger.h"

#include "CommandLineOption.h"
#include "System.h"

int main(int argc, char** argv)
{
    rafi::emu::CommandLineOption option(argc, argv);

    rafi::emu::System system(option.GetXLEN(), option.GetPc(), option.GetRamSize());
    rafi::emu::TraceTextLogger traceTextLogger(option.GetXLEN(), option.GetStateLogPath().c_str(), &system);
    rafi::emu::TraceLogger logger(option.GetXLEN(), option.GetDumpPath().c_str(), &system);
    rafi::emu::Profiler profiler;

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

    if (option.IsTraceTextEnabled())
    {
        traceTextLogger.EnableDump();
    }

    if (option.IsDumpEnabled())
    {
        logger.EnableDump();
    }
    if (option.IsDumpCsrEnabled())
    {
        logger.EnableDumpCsr();
    }
    if (option.IsDumpFpRegEnabled())
    {
        logger.EnableDumpFpReg();
    }
    if (option.IsDumpIntRegEnabled())
    {
        logger.EnableDumpIntReg();
    }
    if (option.IsDumpMemoryEnabled())
    {
        logger.EnableDumpMemory();
    }
    if (option.IsProfileEnabled())
    {
        profiler.Enable();
    }
    if (option.IsHostIoEnabled())
    {
        logger.EnableDumpHostIo();
        system.SetHostIoAddress(option.GetHostIoAddress());
    }

    system.SetDtbAddress(option.GetDtbAddress());

    int cycle;

    try
    {
        for (cycle = 0; cycle < option.GetCycle(); cycle++)
        {
            profiler.Switch(rafi::emu::Profiler::Phase_Dump);
            traceTextLogger.DumpCycle(cycle);

            system.ProcessCycle(&profiler);

            if (cycle >= option.GetDumpSkipCycle())
            {
                profiler.Switch(rafi::emu::Profiler::Phase_Dump);
                logger.DumpCycle(cycle);
            }
            profiler.Switch(rafi::emu::Profiler::Phase_None);

            if (option.IsHostIoEnabled())
            {
                const auto hostIoValue = system.GetHostIoValue();
                if (hostIoValue != 0)
                {
                    break;
                }
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

    profiler.Dump();

    return 0;
}

