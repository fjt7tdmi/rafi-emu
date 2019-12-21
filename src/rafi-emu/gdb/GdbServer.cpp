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

#include <algorithm>
#include <cassert>
#include <cinttypes>
#include <numeric>

#include <rafi/emu.h>
#include <rafi/trace.h>

#include "GdbServer.h"
#include "GdbUtil.h"
#include "GdbTypes.h"

#include "../Socket.h"

namespace rafi { namespace emu {

GdbServer::GdbServer(XLEN xlen, ISystem* pSystem, int port)
    : m_XLEN(xlen)
    , m_pSystem(pSystem)
    , m_Port(port)
    , m_GdbCommandFactory(xlen)
{
    m_ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

    char yes = 1;
    setsockopt(m_ServerSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(m_Port));
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_ServerSocket, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        printf("[gdb] Failed to bind() (error: %d).\n", GetSocketError());
        std::exit(1);
    }

    if (listen(m_ServerSocket, 1) != 0)
    {
        printf("[gdb] Failed to listen() (error: %d).\n", GetSocketError());
        std::exit(1);
    }
}

GdbServer::~GdbServer()
{
    close(m_ServerSocket);
}

void GdbServer::Process()
{
    for (;;)
    {
        struct sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);
        const auto clientSocket = ::accept(m_ServerSocket, (struct sockaddr*)&addr, &addrlen);
        if (clientSocket < 0)
        {
            printf("[gdb] Failed to Accept().\n");
            break;
        }

        ProcessSession(clientSocket);
    }
}

void GdbServer::ProcessSession(int clientSocket)
{
    for (;;)
    {
        char buffer[GdbCommandBufferSize + 1] = {0};

        if (!ReceiveCommand(buffer, GdbCommandBufferSize, clientSocket))
        {
            printf("[gdb] Failed to read command.\n");
            break;
        }

        SendAck(clientSocket);

        auto command = m_GdbCommandFactory.Parse(std::string(buffer));

        auto response = command->Process(m_pSystem, &m_GdbData);

        SendResponse(clientSocket, response);
    }
}

bool GdbServer::ReceiveCommand(char* buffer, size_t bufferSize, int socket)
{
    for (;;)
    {
        char start;
        if (recv(socket, &start, sizeof(start), 0) != sizeof(start))
        {
            return false;
        }
        if (start == '$')
        {
            break;
        }
        else if (start != '+')
        {
            printf("[gdb] Invalid start character (0x%02x).\n", start);
            return false;
        }
    }

    int offset;
    for (offset = 0; offset < bufferSize; offset++)
    {
        if (recv(socket, &buffer[offset], sizeof(char), 0) != sizeof(char))
        {
            return false;
        }
        if (buffer[offset] < 0 || buffer[offset] == 127)
        {
            printf("[gdb] Invalid character (0x%02x).\n", buffer[offset]);
            return false;
        }
        if (buffer[offset] == '#')
        {
            buffer[offset] = '\0';
            break;
        }
    }
    if (offset == bufferSize)
    {
        printf("[gdb] Command buffer overflowed.\n");
        return false;
    }

    printf("[gdb] [recv] %s\n", buffer);

    char checksum[3] = {0};
    if (recv(socket, checksum, 2, 0) != 2)
    {
        return false;
    }
    if (checksum[0] < '0' || 'f' < checksum[0] ||
        checksum[1] < '0' || 'f' < checksum[1])
    {
        printf("[gdb] Invalid checksum (byte0:0x%02x byte1:0x%02x).\n", (uint8_t)checksum[0], (uint8_t)checksum[1]);
        return false;
    }

    const uint8_t sum = (uint8_t)std::accumulate(&buffer[0], &buffer[bufferSize], 0);
    const uint8_t checksumValue = HexToUInt8(checksum);
    if (sum != checksumValue)
    {
        printf("[gdb] Checksum verification error (sum:0x%02" PRIx8 " checksum:0x%02" PRIx8 ").", sum, checksumValue);
        return false;
    }

    return true;
}

void GdbServer::SendAck(int clientSocket)
{
    const auto ack = "+";
    send(clientSocket, ack, (int)strlen(ack), 0);
}

void GdbServer::SendResponse(int clientSocket, const std::string& command)
{
    printf("[gdb] [send] %s\n", command.c_str());

    send(clientSocket, "$", 1, 0);
    send(clientSocket, command.c_str(), command.size(), 0);

    const auto checksum = static_cast<uint8_t>(std::accumulate(command.begin(), command.end(), 0) % 256);

    char str[4] = {0};
    sprintf(str, "#%02" PRIx8, checksum);
    send(clientSocket, str, 3, 0);
}

}}
