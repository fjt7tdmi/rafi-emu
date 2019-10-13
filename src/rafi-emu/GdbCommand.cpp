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

#include <cassert>
#include <cstdlib>
#include <cstring>

#ifdef WIN32
#include <Windows.h>
#include <Winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include <rafi/emu.h>

#include "GdbServer.h"

namespace rafi { namespace emu {

GdbInvalidCommand::~GdbInvalidCommand()
{
}

void GdbInvalidCommand::Process(int socket)
{
    const auto response = "$#00";
    send(socket, response, (int)strlen(response), 0);
}

GdbRegisterReadCommand::~GdbRegisterReadCommand()
{
}

void GdbRegisterReadCommand::Process(int socket)
{
    (void)socket;
    printf("[gdb] TODO: Implement register read command.\n");
    exit(1);
}

GdbMemoryReadCommand::GdbMemoryReadCommand(uint64_t address, uint64_t length)
    : m_Address(address)
    , m_Length(length)
{
}

GdbMemoryReadCommand::~GdbMemoryReadCommand()
{
}

void GdbMemoryReadCommand::Process(int socket)
{
    (void)socket;
    printf("[gdb] TODO: Implement memory read command.\n");
    exit(1);
}

}}
