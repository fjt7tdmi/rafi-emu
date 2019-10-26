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
    static const size_t CommandBufferSize = 256;

    void ProcessSession(int socket);

    bool ReadCommand(char* pOutBuffer, size_t bufferSize, int socket);
    void SendAck(int socket);

    void ProcessCommand(int socket, const std::string& command);
    void ProcessCommandQuery(int socket, const std::string& command);
    void ProcessCommandReadReg(int socket);
    void ProcessCommandReadReg32(int socket);
    void ProcessCommandReadReg64(int socket);

    void SendResponse(int socket, const char* str);
    void SendResponse(int socket, const char* buffer, size_t bufferSize);

    uint8_t HexToUInt8(const char* buffer, size_t bufferSize);

    template <typename T>
    void ToHex(char* pOutBuffer, T value)
    {
        static_assert(std::is_integral_v<T>);
        static_assert(std::is_unsigned_v<T>);

        for (int i = 0; i < sizeof(value) * 2; i += 2)
        {
            const T high = (value % 0x100) / 0x10;
            const T low = value % 0x10;

            pOutBuffer[i] = high < 10 ? '0' + high : 'a' + (high - 10);
            pOutBuffer[i + 1] = low < 10 ? '0' + low : 'a' + (low - 10);

            value >>= 8;
        }
    }

    XLEN m_XLEN;
    System* m_pSystem;

    int m_Port;
    int m_ServerSocket;

    bool m_Started { false };
};

}}
