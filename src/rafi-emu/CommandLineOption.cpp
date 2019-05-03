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

#include <boost/program_options.hpp>

#include <rafi/common.h>

#include "CommandLineOption.h"

namespace po = boost::program_options;

namespace rafi { namespace emu {

namespace {

class CommandLineOptionException
{
public:
    explicit CommandLineOptionException(const char *pArgument)
        : CommandLineOptionException(pArgument, nullptr)
    {
    }

    CommandLineOptionException(const char *pArgument, const char* pMessage)
        : m_pArgument(pArgument)
        , m_pMessage(pMessage)
    {
    }

    virtual void PrintMessage() const
    {
        std::cout << "[CommandLineOptionException] " << m_pArgument;
        if (m_pMessage != nullptr)
        {
            std::cout << " (" << m_pMessage << ")";
        }
        std::cout << std::endl;
    }

private:
    const char* m_pArgument;
    const char* m_pMessage;
};

}

LoadOption::LoadOption(const std::string& arg)
{
    const auto delimPos = arg.find(':');

    if (delimPos == std::string::npos)
    {
        throw CommandLineOptionException(arg.c_str(), "Failed to parse <file:address> pair.");
    }

    m_Path = arg.substr(0, delimPos);

    try
    {
        m_Address = std::strtoull(arg.substr(delimPos + 1).c_str(), nullptr, 16);
    }
    catch (std::out_of_range&)
    {
        throw CommandLineOptionException(arg.c_str(), "Failed to parse address.");
    }

    if (m_Address == 0)
    {
        throw CommandLineOptionException(arg.c_str(), "Failed to parse address or address is 0.");
    }
}

const std::string& LoadOption::GetPath() const
{
    return m_Path;
}

uint64_t LoadOption::GetAddress() const
{
    return m_Address;
}

CommandLineOption::CommandLineOption(int argc, char** argv)
{
    po::options_description desc("options");
    desc.add_options()
        ("cycle", po::value<int>(&m_Cycle)->default_value(0), "number of emulation cycles")
        ("dump-path", po::value<std::string>(), "path of dump file")
        ("dump-skip-cycle", po::value<int>(&m_DumpSkipCycle)->default_value(0), "number of cycles to skip dump")
        ("enable-dump-csr", "output csr contents to dump file")
        ("enable-dump-memory", "output memory contents to dump file")
        ("load", po::value<std::vector<std::string>>(), "path of binary file which is loaded to memory")
        ("help", "show help")
        ("host-io-addr", po::value<std::string>(), "host io address (hex)")
        ("dtb-addr", po::value<std::string>(), "dtb address (hex)")
        ("pc", po::value<std::string>(), "initial program counter value (hex)")
        ("ram-size", po::value<size_t>(&m_RamSize)->default_value(DefaultRamSize), "ram size (byte)")
        ("xlen", po::value<int>(), "XLEN");

    po::variables_map variables;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), variables);
    }
    catch (const boost::program_options::error_with_option_name& e)
    {
        std::cout << e.what() << std::endl;
        std::exit(1);
    }
    po::notify(variables);

    if (variables.count("help"))
    {
        std::cout << desc << std::endl;
        std::exit(0);
    }

    m_DumpEnabled = variables.count("dump-path") > 0;
    m_DumpCsrEnabled = variables.count("enable-dump-csr") > 0;
    m_DumpMemoryEnabled = variables.count("enable-memory-csr") > 0;
    m_HostIoEnabled = variables.count("host-io-addr") > 0;

    if (variables.count("dtb-addr"))
    {
        m_DtbAddress = strtoull(variables["dtb-addr"].as<std::string>().c_str(), 0, 16);
    }
    if (variables.count("host-io-addr"))
    {
        m_HostIoAddress = strtoull(variables["host-io-addr"].as<std::string>().c_str(), 0, 16);
    }
    if (variables.count("pc"))
    {
        m_Pc = strtoull(variables["pc"].as<std::string>().c_str(), 0, 16);
    }
    if (variables.count("dump-path"))
    {
        m_DumpPath = variables["dump-path"].as<std::string>();
    }

    try
    {
        for (auto& str: variables["load"].as<std::vector<std::string>>())
        {
            m_LoadOptions.emplace_back(str);
        }
    }
    catch (CommandLineOptionException e)
    {
        e.PrintMessage();
        exit(1);
    }

    if (variables.count("xlen"))
    {
        switch (variables["xlen"].as<int>())
        {
        case 32:
            m_XLEN = XLEN::XLEN32;
            break;
        case 64:
            m_XLEN = XLEN::XLEN64;
            break;
        default:
            std::cout << "--xlen must be 32 or 64." << std::endl;
            std::exit(0);
            break;
        }
    }
}

bool CommandLineOption::IsDumpEnabled() const
{
    return m_DumpEnabled;
}

bool CommandLineOption::IsDumpCsrEnabled() const
{
    return m_DumpCsrEnabled;
}

bool CommandLineOption::IsDumpMemoryEnabled() const
{
    return m_DumpMemoryEnabled;
}

bool CommandLineOption::IsHostIoEnabled() const
{
    return m_HostIoEnabled;
}

const std::string& CommandLineOption::GetDumpPath() const
{
    return m_DumpPath;
}

const std::vector<LoadOption>& CommandLineOption::GetLoadOptions() const
{
    return m_LoadOptions;
}

XLEN CommandLineOption::GetXLEN() const
{
    return m_XLEN;
}

int CommandLineOption::GetCycle() const
{
    return m_Cycle;
}

int CommandLineOption::GetDumpSkipCycle() const
{
    return m_DumpSkipCycle;
}

size_t CommandLineOption::GetRamSize() const
{
    return m_RamSize;
}

uint64_t CommandLineOption::GetDtbAddress() const
{
    return m_DtbAddress;
}

uint64_t CommandLineOption::GetHostIoAddress() const
{
    return m_HostIoAddress;
}

uint64_t CommandLineOption::GetPc() const
{
    return m_Pc;
}

}}
