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

#include <rafi/emu.h>

#include "../IEmulator.h"

#include "GdbCommandFactory.h"

namespace rafi { namespace emu {

class GdbServer
{
public:
    explicit GdbServer(XLEN xlen, IEmulator* pEmulator, int port);
    ~GdbServer();

    void Process();

private:
    void ProcessSession(int clientSocket);

    bool ReceiveCommand(char* pOutBuffer, size_t bufferSize, int clientSocket);

    void SendAck(int clientSocket);
    void SendResponse(int clientSocket, const std::string& response);

    XLEN m_XLEN;
    IEmulator* m_pEmulator;
    int m_Port;

    GdbCommandFactory m_GdbCommandFactory;
    GdbData m_GdbData;

    int m_ServerSocket;
};

}}
