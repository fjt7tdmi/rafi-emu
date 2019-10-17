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

#include <rafi/emu.h>

#include "GdbCommand.h"

namespace rafi { namespace emu {

class GdbServer
{
public:
    explicit GdbServer(int port);
    ~GdbServer();

    void Start();
    void Stop();

    void Process();

private:
    static const size_t CommandBufferSize = 256;

    void ProcessSession(int socket);

    bool ReadCommand(char* buffer, size_t bufferSize, int socket);
    void SendAck(int socket);

    std::unique_ptr<GdbCommand> ParseCommand(const char* buffer, size_t bufferSize);
    std::unique_ptr<GdbCommand> ParseCommandQuery(const char* buffer, size_t bufferSize);

    uint8_t HexToUInt8(const char* buffer, size_t bufferSize);

    int m_Port;
    int m_ServerSocket;

    bool m_Started { false };
};

}}
