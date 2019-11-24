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

#pragma once

#include <map>
#include <memory>
#include <type_traits>

#include <rafi/emu.h>

#include "System.h"

namespace rafi { namespace emu {

class GdbServer
{
public:
    explicit GdbServer(XLEN xlen, System* pSystem, int port);
    ~GdbServer();

    void Start();
    void Stop();

    void Process();

private:
    static const uint32_t BreakInsn = 0x100073;
    static const size_t CommandBufferSize = 256;

    void ProcessSession(int socket);

    bool ReadCommand(char* pOutBuffer, size_t bufferSize, int socket);
    void SendAck(int socket);

    void ProcessCommand(int socket, const std::string& command);
    void ProcessCommandInsertBreakPoint(int socket, const std::string& command);
    void ProcessCommandReadReg(int socket);
    void ProcessCommandReadReg32(int socket);
    void ProcessCommandReadReg64(int socket);
    void ProcessCommandReadMemory(int socket, const std::string& command);
    void ProcessCommandRemoveBreakPoint(int socket, const std::string& command);
    void ProcessCommandQuery(int socket, const std::string& command);
    void ProcessCommandStep(int socket);
    void ProcessCommandVerbose(int socket, const std::string& command);

    void SendResponse(int socket, const char* str);

    uint8_t HexToUInt8(const char* buffer, size_t bufferSize);
    
    uint64_t ParseHex(const std::string& str);

    template <typename T>
    void ToHex(char* pOutBuffer, size_t bufferSize, const T value)
    {
        (void)bufferSize; // for release build

        static_assert(std::is_integral_v<T>);
        static_assert(std::is_unsigned_v<T>);

        auto tmp = value;

        for (int i = 0; i < sizeof(tmp) * 2; i += 2)
        {
            const T high = (tmp % 0x100) / 0x10;
            const T low = tmp % 0x10;

            assert(i + 1 <= bufferSize);

            pOutBuffer[i] = high < 10 ? '0' + high : 'a' + (high - 10);
            pOutBuffer[i + 1] = low < 10 ? '0' + low : 'a' + (low - 10);

            tmp >>= 8;
        }
    }

    template <>
    void ToHex(char* pOutBuffer, size_t bufferSize, const std::string str)
    {
        ToHex(pOutBuffer, bufferSize, str.c_str());
    }

    template <>
    void ToHex(char* pOutBuffer, size_t bufferSize, const char* str)
    {
        (void)bufferSize; // for release build

        for (int i = 0; i < std::strlen(str); i++)
        {
            const char high = (str[i] % 0x100) / 0x10;
            const char low = str[i] % 0x10;

            assert(i * 2 + 1 <= bufferSize);

            pOutBuffer[i * 2] = high < 10 ? '0' + high : 'a' + (high - 10);
            pOutBuffer[i * 2 + 1] = low < 10 ? '0' + low : 'a' + (low - 10);
        }
    }

    std::map<paddr_t, uint32_t> m_MemoryBackups;

    XLEN m_XLEN;
    System* m_pSystem;

    int m_Port;
    int m_ServerSocket;

    bool m_Started { false };
};

}}
