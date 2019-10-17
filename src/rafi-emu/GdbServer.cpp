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
#include <numeric>

#ifdef WIN32
#include <Windows.h>
#include <Winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include <rafi/emu.h>

#include "GdbCommand.h"
#include "GdbServer.h"

namespace rafi { namespace emu {

GdbServer::GdbServer(int port)
    : m_Port(port)
{
}

GdbServer::~GdbServer()
{
    Stop();
}

void GdbServer::Start()
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
#ifdef WIN32
        printf("[gdb] Failed to bind() (error: %d).\n", GetLastError());
#else
        printf("[gdb] Failed to bind().\n");
#endif
        std::exit(1);
    }

    if (listen(m_ServerSocket, 1) != 0)
    {
#ifdef WIN32
        printf("[gdb] Failed to listen() (error: %d).\n", GetLastError());
#else
        printf("[gdb] Failed to listen().\n");
#endif
        std::exit(1);
    }

    m_Started = true;
}

void GdbServer::Stop()
{
#ifdef WIN32
    ::closesocket(m_ServerSocket);
#else
    ::close(m_ServerSocket);
#endif

    m_Started = false;
}

void GdbServer::Process()
{
    assert(m_Started);

    for (;;)
    {
        struct sockaddr_in addr;
        int addrlen = sizeof(addr);
        const auto clientSocket = ::accept(m_ServerSocket, (struct sockaddr*)&addr, &addrlen);
        if (clientSocket < 0)
        {
            printf("[gdb] Failed to Accept().\n");
            break;
        }

        ProcessSession(clientSocket);
    }
}

void GdbServer::ProcessSession(int socket)
{
    printf("GdbServer::ProcessSession()\n");

    for (;;)
    {
        char buffer[CommandBufferSize];

        if (!ReadCommand(buffer, sizeof(buffer), socket))
        {
            printf("[gdb] Failed to read command.\n");
            break;
        }

        auto command = ParseCommand(buffer, sizeof(buffer));
        if (!command)
        {
            printf("[gdb] Failed to parse command.\n");
            break;
        }

        SendAck(socket);
        command->Process(socket);
    }
}

bool GdbServer::ReadCommand(char* buffer, size_t bufferSize, int socket)
{
    memset(buffer, 0, bufferSize);

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

    printf("[gdb] request: $%s#\n", buffer);

    char checksum[2];
    if (recv(socket, checksum, sizeof(checksum), 0) != sizeof(checksum))
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
    const uint8_t checksumValue = HexToUInt8(checksum, sizeof(checksum));
    if (sum != checksumValue)
    {
        printf("[gdb] Checksum verification error (sum:0x%02" PRIx8 " checksum:0x%02" PRIx8 ").", sum, checksumValue);
        return false;
    }

    return true;
}

std::unique_ptr<GdbCommand> GdbServer::ParseCommand(const char* buffer, size_t bufferSize)
{
    if (bufferSize < 1)
    {
        return std::make_unique<GdbInvalidCommand>();
    }

    // TODO: Implement g,m commands
    switch (buffer[0])
    {
    case 'q':
        return ParseCommandQuery(buffer, bufferSize);
    default:
        return std::make_unique<GdbInvalidCommand>();
    }
}

std::unique_ptr<GdbCommand> GdbServer::ParseCommandQuery(const char* buffer, size_t bufferSize)
{
    (void)bufferSize;

    auto query = std::string(buffer);
    auto pos = query.find(':');

    auto name = query.substr(0, pos);

    if (name == "qSupported")
    {
        return std::make_unique<GdbQuerySupportedCommand>(CommandBufferSize);
    }
    else
    {
        return std::make_unique<GdbInvalidCommand>();
    }

}

void GdbServer::SendAck(int socket)
{
    const auto ack = "+";
    send(socket, ack, (int)strlen(ack), 0);
}

uint8_t GdbServer::HexToUInt8(const char* buffer, size_t bufferSize)
{
    uint8_t sum = 0;

    for (size_t i = 0; i < bufferSize; i++)
    {
        sum <<= 4;

        if ('0' <= buffer[i] && buffer[i] <= '9')
        {
            sum += static_cast<uint8_t>(buffer[i] - '0');
        }
        else if ('a' <= buffer[i] && buffer[i] <= 'f')
        {
            sum += static_cast<uint8_t>(buffer[i] - 'a' + 10);
        }
        else
        {
            printf("[gdb] input is not hex.\n");
            exit(1);
        }
    }

    return sum;
}

}}
