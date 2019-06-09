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

namespace rafi {

namespace {
    static const int DefaultCycleCount = 1000 * 1000 * 1000;
    static const int DefaultThreshold = 10;
}

CommandLineOption::CommandLineOption(int argc, char** argv)
{
    po::options_description desc("options");
    desc.add_options()
        ("expect,e", po::value<std::string>(&m_ExpectPath)->required(), "expect trace binary")
        ("actual,a", po::value<std::string>(&m_ActualPath)->required(), "actual trace binary")
        ("check-physical-pc,p", "enable comparing physical PC")
        ("count,c", po::value<int>(&m_CycleCount)->default_value(DefaultCycleCount), "number of cycles to print")
        ("threshold,t", po::value<int>(&m_Threshold)->default_value(DefaultThreshold), "threshold to stop somparation")
        ("help,h", "show help");

    po::positional_options_description posOptDesc;
    posOptDesc.add("expect", 1);
    posOptDesc.add("actual", 1);

    po::variables_map variables;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), variables);
        po::notify(variables);
    }
    catch (const boost::program_options::error_with_option_name& e)
    {
        std::cout << e.what() << std::endl;
        std::exit(1);
    }

    if (variables.count("help"))
    {
        std::cout << desc << std::endl;
        std::exit(0);
    }

    m_CheckPhysicalPc = variables.count("check-physical-pc") > 0;
}

const std::string& CommandLineOption::GetExpectPath() const
{
    return m_ExpectPath;
}

const std::string& CommandLineOption::GetActualPath() const
{
    return m_ActualPath;
}

int CommandLineOption::GetCycleCount() const
{
    return m_CycleCount;
}

int CommandLineOption::GetThreshold() const
{
    return m_Threshold;
}

bool CommandLineOption::CheckPhysicalPc() const
{
    return m_CheckPhysicalPc;
}

}
