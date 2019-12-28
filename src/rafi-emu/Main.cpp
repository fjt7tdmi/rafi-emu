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
#include "gdb/GdbServer.h"

#include "CommandLineOption.h"
#include "Socket.h"
#include "Emulator.h"
#include "TraceLogger.h"

int main(int argc, char** argv)
{
    rafi::emu::CommandLineOption option(argc, argv);

    rafi::emu::Emulator emulator(option.GetXLEN(), option.GetPc(), option.GetRamSize());
    rafi::emu::TraceLogger logger(option.GetXLEN(), option.GetTraceLoggerConfig(), emulator.GetSystem());

    try
    {
        for (auto& loadOption: option.GetLoadOptions())
        {
            emulator.GetSystem()->LoadFileToMemory(loadOption.GetPath().c_str(), loadOption.GetAddress());
        }
    }
    catch (rafi::FileOpenFailureException e)
    {
        e.PrintMessage();
        std::exit(1);
    }

    if (option.IsHostIoEnabled())
    {
        emulator.GetSystem()->SetHostIoAddress(option.GetHostIoAddress());
    }

    emulator.GetSystem()->SetDtbAddress(option.GetDtbAddress());

    int cycle = 0;

    try
    {
        for (; cycle < option.GetCycle(); cycle++)
        {
            const bool dumpEnabled = cycle >= option.GetDumpSkipCycle();
            const bool lastCycle = option.IsHostIoEnabled() && emulator.GetSystem()->GetHostIoValue() != 0;

            if (dumpEnabled)
            {
                logger.BeginCycle(cycle, emulator.GetSystem()->GetPc());
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

            emulator.GetSystem()->ProcessCycle();

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
        emulator.GetSystem()->PrintStatus();
    }

    std::cout << "Emulation finished @ cycle "
        << std::dec << cycle
        << std::hex << " (0x" << cycle << ")" << std::endl;

    if (option.IsGdbEnabled())
    {
        rafi::emu::InitializeSocket();

        std::cout << "Start gdb server." << std::endl;

        rafi::emu::GdbServer gdbServer(option.GetXLEN(), &emulator, option.GetGdbPort());
        gdbServer.Process();

        rafi::emu::FinalizeSocket();
    }

    return 0;
}

