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

#include <boost/program_options.hpp>

#include <emu/BasicTypes.h>
#include <emu/Exception.h>

#include "bus/Bus.h"

#include "log/Profiler.h"
#include "log/TraceDumper.h"

#include "System.h"

using namespace std;

namespace po = boost::program_options;

namespace {

class BinaryOption
{
public:
    explicit BinaryOption(const string& arg)
    {
        const auto delimPos = arg.find(':');

        if (delimPos == string::npos)
        {
            m_Path = arg;
            m_Address = DefaultAddress;
        }
        else
        {
            m_Path = arg.substr(0, delimPos);

            try
            {
                m_Address = strtoull(arg.substr(delimPos + 1).c_str(), nullptr, 16);
            }
            catch (out_of_range&)
            {
                throw CommandLineOptionException(arg.c_str(), "Failed to parse address.");
            }

            if (m_Address == 0)
            {
                throw CommandLineOptionException(arg.c_str(), "Failed to parse address or address is 0.");
            }
        }
    }

    const string& GetPath() const
    {
        return m_Path;
    }

    PhysicalAddress GetAddress() const
    {
        return m_Address;
    }

private:
    static const PhysicalAddress DefaultAddress{ Bus::MemoryAddr };

    string m_Path;
    PhysicalAddress m_Address;
};

uint32_t GetHexProgramOption(const po::variables_map& vm, const char* optionName, uint32_t defaultValue)
{
    try
    {
        if (vm.count(optionName))
        {
            return stoul(vm[optionName].as<string>(), 0, 16);
        }
        else
        {
            return defaultValue;
        }
    }
    catch (const invalid_argument&)
    {
        throw CommandLineOptionException(optionName, "invalid argument");
    }
    catch (const out_of_range&)
    {
        throw CommandLineOptionException(optionName, "out of range");
    }
}

}

int main(int argc, char** argv)
{
    const uint32_t DefaultInitialPc = 0x80000000;

    int cycle;
    int dumpSkipCycle;

    po::options_description optionDesc("options");
    optionDesc.add_options()
        ("binary", po::value<vector<string>>(), "path of binary file which is loaded to memory")
        ("cycle", po::value<int>(&cycle)->default_value(0), "number of emulation cycles")
        ("dtb-address", po::value<string>(), "dtb physical address")
        ("dump-path", po::value<string>(), "path of dump file")
        ("dump-skip-cycle", po::value<int>(&dumpSkipCycle)->default_value(0), "number of cycles to skip dump")
        ("enable-dump-memory", "output memory contents to dump file")
        ("pc", po::value<string>(), "initial program counter value")
        ("stop-by-host-io", "stop emulation when host io value is changed")
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
    po::notify(optionMap);

    if (optionMap.count("help"))
    {
        std::cout << optionDesc << std::endl;
        exit(0);
    }

    uint32_t initialPc;
    uint32_t dtbAddress;
    try
    {
        initialPc = GetHexProgramOption(optionMap, "pc", DefaultInitialPc);
        dtbAddress = GetHexProgramOption(optionMap, "dtb-address", 0);
    }
    catch (CommandLineOptionException e)
    {
        e.PrintMessage();
        exit(1);
    }

    auto pProfiler = new Profiler();
    auto pSystem = new System(initialPc);

    pSystem->SetupDtbAddress(static_cast<int32_t>(dtbAddress));

    TraceDumper* dumper;
    try
    {
        for (auto& arg: optionMap["binary"].as<vector<string>>())
        {
            BinaryOption binaryOption(arg);
            pSystem->LoadFileToMemory(binaryOption.GetPath().c_str(), binaryOption.GetAddress());
        }

        dumper = new TraceDumper(optionMap["dump-path"].as<string>().c_str(), pSystem);
    }
    catch (CommandLineOptionException e)
    {
        e.PrintMessage();
        exit(1);
    }
    catch (FileOpenFailureException e)
    {
        e.PrintMessage();
        exit(1);
    }

    if (optionMap.count("dump-path"))
    {
        dumper->EnableDump();
    }
    if (optionMap.count("enable-dump-memory"))
    {
        dumper->EnableDumpMemory();
    }

    dumper->DumpHeader();

    try
    {
        for (cycle = 0; cycle < optionMap["cycle"].as<int>(); cycle++)
        {
            pProfiler->SwitchPhase(Profiler::Phase_Process);
            pSystem->ProcessOneCycle();

            pProfiler->SwitchPhase(Profiler::Phase_Dump);
            dumper->DumpOneCycle(cycle);

            pProfiler->SwitchPhase(Profiler::Phase_None);
            if (optionMap.count("stop-by-host-io"))
            {
                const auto hostIoValue = pSystem->GetHostIoValue();
                if (hostIoValue != 0)
                {
                    break;
                }
            }
        }
    }
    catch (NotImplementedException e)
    {
        e.PrintMessage();
    }
    catch (FatalException e)
    {
        e.PrintMessage();
    }
    catch (InvalidAccessException e)
    {
        e.PrintMessage();
    }

    dumper->DumpFooter();

    pProfiler->Dump();

    delete dumper;
    delete pSystem;
    delete pProfiler;

    std::cout << "Emulation finished @ cycle "
        << std::dec << cycle
        << std::hex << " (0x" << cycle << ")" << std::endl;

    return 0;
}

