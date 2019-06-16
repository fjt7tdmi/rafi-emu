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

namespace rafi { namespace dump {

namespace {
    static const int DefaultCycleCount = 1000 * 1000 * 1000;
    static const int DefaultCycleEnd = DefaultCycleCount;
}

CommandLineOption::CommandLineOption(int argc, char** argv)
{
    po::options_description optDesc("options");
    optDesc.add_options()
        ("begin,b", po::value<int>(&m_CycleBegin)->default_value(0), "cycle to begin printing")
        ("count,c", po::value<int>(&m_CycleCount)->default_value(DefaultCycleCount), "number of cycles to print")
        ("end,e", po::value<int>(&m_CycleEnd)->default_value(DefaultCycleEnd), "cycle to end printing")
        ("filter,f", po::value<std::string>(&m_FilterDescription), "description of cycle print filter")
        ("input,i", po::value<std::string>(&m_Path), "input trace binary path")
        ("json,j", "set output format as json")
        ("help,h", "show help");

    po::positional_options_description posOptDesc;
    posOptDesc.add("input", -1);

    po::variables_map optMap;
    try
    {
        po::store(po::command_line_parser(argc, argv).options(optDesc).positional(posOptDesc).run(), optMap);
        po::notify(optMap);
    }
    catch (const boost::program_options::error_with_option_name& e)
    {
        std::cout << e.what() << std::endl;
        std::exit(1);
    }

    if (optMap.count("help") > 0 || optMap.count("input") == 0)
    {
        std::cout << optDesc << std::endl;
        std::exit(0);
    }

    m_PrinterType = (optMap.count("json") > 0) ? PrinterType::Json : PrinterType::Text;
}

PrinterType CommandLineOption::GetPrinterType() const
{
    return m_PrinterType;
}

const std::string& CommandLineOption::GetFilterDescription() const
{
    return m_FilterDescription;
}

const std::string& CommandLineOption::GetPath() const
{
    return m_Path;
}

const int CommandLineOption::GetCycleBegin() const
{
    return m_CycleBegin;
}

const int CommandLineOption::GetCycleCount() const
{
    return m_CycleCount;
}

const int CommandLineOption::GetCycleEnd() const
{
    return m_CycleEnd;
}

}}
