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

CommandLineOption::CommandLineOption(int argc, char** argv)
{
    po::options_description desc("options");
    desc.add_options()
        ("check-physical-pc", "enable comparing physical PC")
        ("expect", po::value<std::string>(&m_ExpectPath)->required(), "expect trace binary")
        ("actual", po::value<std::string>(&m_ActualPath)->required(), "actual trace binary")
        ("help", "show help");

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

bool CommandLineOption::CheckPhysicalPc() const
{
    return m_CheckPhysicalPc;
}

}
