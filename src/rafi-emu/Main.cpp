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

int main(int argc, char** argv)
{
    rafi::emu::CommandLineOption option(argc, argv);
    rafi::emu::Emulator emulator(option);

    try
    {
        for (auto& loadOption: option.GetLoadOptions())
        {
            emulator.LoadFileToMemory(loadOption.GetPath().c_str(), loadOption.GetAddress());
        }
    }
    catch (rafi::FileOpenFailureException e)
    {
        e.PrintMessage();
        std::exit(1);
    }

    try
    {
        const auto condition = option.IsHostIoEnabled()
            ? rafi::emu::EmulationStop_HostIo
            : rafi::emu::EmulationStop_None;

        emulator.Process(condition, option.GetCycle());
    }
    catch (rafi::emu::RafiEmuException)
    {
        std::cout << "Emulation stopped by exception." << std::endl;
        emulator.PrintStatus();        
        std::exit(1);
    }

    std::cout << "Emulation finished @ cycle "
        << std::dec << emulator.GetCycle()
        << std::hex << " (0x" << emulator.GetCycle() << ")" << std::endl;

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

