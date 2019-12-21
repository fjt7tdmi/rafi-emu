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

#include <string>

#include <rafi/emu.h>

#include "GdbCommandFactory.h"
#include "GdbException.h"

namespace rafi { namespace emu {

GdbCommandFactory::GdbCommandFactory(XLEN xlen)
    : m_XLEN(xlen)
{
}

std::unique_ptr<IGdbCommand> GdbCommandFactory::Parse(const std::string& cmd)
{
    if (cmd.length() < 1)
    {
        return std::make_unique<GdbCommandInvalid>();
    }

    switch (cmd[0])
    {
    case '?':
        return std::make_unique<GdbCommandStopReason>();
    case 'H':
        return std::make_unique<GdbCommandSetThread>();
    case 'Z':
        return std::make_unique<GdbCommandInsertBreakPoint>(cmd);
    case 'c':
        return std::make_unique<GdbCommandContinue>();
    case 'g':
        return std::make_unique<GdbCommandReadReg>(m_XLEN);
    case 'm':
        return std::make_unique<GdbCommandReadMemory>(cmd);
    case 'q':
        return std::make_unique<GdbCommandQuery>(cmd);
    case 's':
        return std::make_unique<GdbCommandStep>();
    case 'v':
        return ParseLongCommand(cmd);
    case 'z':
        return std::make_unique<GdbCommandRemoveBreakPoint>(cmd);
    default:
        fprintf(stderr, "Failed to parse gdb command.\n");
        fprintf(stderr, "%s\n", cmd.c_str());
        RAFI_NOT_IMPLEMENTED();
    }
}

std::unique_ptr<IGdbCommand> GdbCommandFactory::ParseLongCommand(const std::string& cmd)
{
    if (cmd == "vMustReplyEmpty")
    {
        return std::make_unique<GdbCommandInvalid>();
    }
    else if (cmd == "vCont?")
    {
        return std::make_unique<GdbCommandContinueQuery>();
    }
    else
    {
        fprintf(stderr, "Failed to parse gdb command.\n");
        fprintf(stderr, "%s\n", cmd.c_str());
        RAFI_NOT_IMPLEMENTED();
    }
}

}}
