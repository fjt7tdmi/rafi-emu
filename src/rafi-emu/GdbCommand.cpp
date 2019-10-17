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
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <numeric>

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

namespace {

void SendResponse(int socket, const char* buffer, size_t bufferSize)
{
    send(socket, "$", 1, 0);
    send(socket, buffer, (int)bufferSize, 0);

    const auto checksum = static_cast<uint8_t>(std::accumulate(buffer, &buffer[bufferSize], 0) % 256);

    char str[4] = {0};
    sprintf(str, "#%02" PRIx8, checksum);
    send(socket, str, 3, 0);
}

}

GdbInvalidCommand::~GdbInvalidCommand()
{
}

void GdbInvalidCommand::Process(int socket)
{
    SendResponse(socket, "", 0);
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

GdbQuerySupportedCommand::GdbQuerySupportedCommand(size_t packetSize)
    : m_PacketSize(packetSize)
{
}

GdbQuerySupportedCommand::~GdbQuerySupportedCommand()
{
}

void GdbQuerySupportedCommand::Process(int socket)
{
    char response[20] = {0};
    sprintf(response, "PacketSize=%zx", m_PacketSize);
    SendResponse(socket, response, strlen(response));
}

}}
