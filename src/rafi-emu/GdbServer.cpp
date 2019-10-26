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

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#include <Winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include <rafi/emu.h>
#include <rafi/trace.h>

#include "GdbServer.h"

namespace rafi { namespace emu {

GdbServer::GdbServer(XLEN xlen, System* pSystem, int port)
    : m_XLEN(xlen)    
    , m_pSystem(pSystem)
    , m_Port(port)
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
        char buffer[CommandBufferSize] = {0};

        if (!ReadCommand(buffer, sizeof(buffer), socket))
        {
            printf("[gdb] Failed to read command.\n");
            break;
        }

        SendAck(socket);

        auto command = std::string(buffer);
        ProcessCommand(socket, command);
    }
}

bool GdbServer::ReadCommand(char* buffer, size_t bufferSize, int socket)
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

void GdbServer::SendAck(int socket)
{
    const auto ack = "+";
    send(socket, ack, (int)strlen(ack), 0);
}

void GdbServer::ProcessCommand(int socket, const std::string& command)
{
    if (command.length() < 1)
    {
        SendResponse(socket, "");
        return;
    }

    // TODO: Implement g,m commands
    switch (command[0])
    {
    case 'g':
        ProcessCommandReadReg(socket);
        break;
    case 'm':
        ProcessCommandReadMemory(socket, command);
        break;
    case 'q':
        ProcessCommandQuery(socket, command);
        break;
    case 'H':
        // rafi-emu supports only thread 0, but always returns 'OK' for H command.
        SendResponse(socket, "OK");
        break;
    case '?':
        SendResponse(socket, "S05"); // 05: SIGTRAP
        break;
    default:
        SendResponse(socket, "");
        break;
    } 
}

void GdbServer::ProcessCommandQuery(int socket, const std::string& command)
{
    auto pos = command.find(':');
    auto name = command.substr(0, pos);

    if (name == "qSupported")
    {
        char response[20] = {0};
        sprintf(response, "PacketSize=%zx", CommandBufferSize);
        SendResponse(socket, response);
    }
    else if (name == "qfThreadInfo")
    {
        SendResponse(socket, "mp01.01"); // pid 1, tid 1
    }
    else if (name == "qsThreadInfo")
    {
        SendResponse(socket, "l"); // End of thread list
    }
    else if (name == "qC")
    {
        SendResponse(socket, "QCp01.01"); // pid 1, tid 1
    }
    else if (name == "qAttached")
    {
        SendResponse(socket, "1");
    }
    else
    {
        SendResponse(socket, "");
    }
}

void GdbServer::ProcessCommandReadReg(int socket)
{
    switch (m_XLEN)
    {
    case XLEN::XLEN32:
        ProcessCommandReadReg32(socket);
        break;
    case XLEN::XLEN64:
        ProcessCommandReadReg64(socket);
        break;    
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }
}

void GdbServer::ProcessCommandReadReg32(int socket)
{
    char buffer[sizeof(uint32_t) * 2 * 33 + sizeof(uint64_t) * 2 * 32];

    // IntReg
    trace::NodeIntReg32 intReg; 
    m_pSystem->CopyIntReg(&intReg);
    for (int i = 0; i < 32; i++)
    {
        const auto offset = sizeof(uint32_t) * 2 * i;
        ToHex(&buffer[offset], intReg.regs[i]);
    }

    // PC
    const uint32_t pc = static_cast<uint32_t>(m_pSystem->GetPc());
    {
        const auto offset = sizeof(uint32_t) * 32;
        ToHex(&buffer[offset], pc);
    }

    // FpReg
    trace::NodeFpReg fpReg; 
    m_pSystem->CopyFpReg(&fpReg, sizeof(fpReg));
    for (int i = 0; i < 32; i++)
    {
        const auto offset = sizeof(uint32_t) * 2 * 33 + sizeof(uint64_t) * 2 * i;
        ToHex(&buffer[offset], fpReg.regs[i].u64.value);
    }

    SendResponse(socket, buffer, sizeof(buffer));
}

void GdbServer::ProcessCommandReadReg64(int socket)
{
    char buffer[sizeof(uint64_t) * 2 * 33 + sizeof(uint64_t) * 2 * 32];

    // IntReg
    trace::NodeIntReg64 intReg; 
    m_pSystem->CopyIntReg(&intReg);
    for (int i = 0; i < 32; i++)
    {
        const auto offset = sizeof(uint64_t) * 2 * i;
        ToHex(&buffer[offset], intReg.regs[i]);
    }

    // PC
    const uint64_t pc = static_cast<uint64_t>(m_pSystem->GetPc());
    {
        const auto offset = sizeof(uint64_t) * 2 * 32;
        ToHex(&buffer[offset], pc);
    }

    // FpReg
    trace::NodeFpReg fpReg; 
    m_pSystem->CopyFpReg(&fpReg, sizeof(fpReg));
    for (int i = 0; i < 32; i++)
    {
        const auto offset = sizeof(uint64_t) * 2 * 33 + sizeof(uint64_t) * 2 * i;
        ToHex(&buffer[offset], fpReg.regs[i].u64.value);
    }

    SendResponse(socket, buffer, sizeof(buffer));
}

void GdbServer::ProcessCommandReadMemory(int socket, const std::string& command)
{
    const auto pos = command.find(',');
    if (pos == std::string::npos)
    {
        SendResponse(socket, "");
        return;
    }

    const auto addr = static_cast<paddr_t>(ParseHex(command.substr(1, pos - 1)));
    const auto size = static_cast<size_t>(ParseHex(command.substr(pos + 1)));

    auto data = reinterpret_cast<uint8_t*>(malloc(size));
    auto response = reinterpret_cast<char*>(malloc(size * 2));

    try
    {
        m_pSystem->ReadMemory(data, size, addr);
        
        for (size_t i = 0; i < size; i++)
        {
            const uint8_t high = data[i] / 0x10;
            const uint8_t low = data[i] % 0x10;

            response[i * 2] = high < 10 ? '0' + high : 'a' + (high - 10);
            response[i * 2 + 1] = low < 10 ? '0' + low : 'a' + (low - 10);
        }

        SendResponse(socket, response, size * 2); // MEMORY_ERROR
    }
    catch (const RafiEmuException&)
    {
        SendResponse(socket, "E08"); // MEMORY_ERROR
    }

    free(data);
    free(response);
}

void GdbServer::SendResponse(int socket, const char* str)
{
    SendResponse(socket, str, strlen(str));
}

void GdbServer::SendResponse(int socket, const char* buffer, size_t bufferSize)
{
    send(socket, "$", 1, 0);
    send(socket, buffer, (int)bufferSize, 0);

    const auto checksum = static_cast<uint8_t>(std::accumulate(buffer, &buffer[bufferSize], 0) % 256);

    char str[4] = {0};
    sprintf(str, "#%02" PRIx8, checksum);
    send(socket, str, 3, 0);
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

uint64_t GdbServer::ParseHex(const std::string& str)
{
    uint64_t sum = 0;

    for (size_t i = 0; i < str.length(); i += 2)
    {
        sum <<= 8;

        const auto digit = std::min(str.length() - i, static_cast<size_t>(2));
        sum += HexToUInt8(&str[i], digit);
    }

    return sum;
}

}}
