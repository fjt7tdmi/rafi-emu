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

namespace rafi { namespace emu {

class GdbCommand
{
public:
    virtual ~GdbCommand()
    {
    }

    virtual void Process(int socket) = 0;
};

class GdbInvalidCommand final : public GdbCommand
{
public:
    virtual ~GdbInvalidCommand() override;
    virtual void Process(int socket) override;
};

class GdbRegisterReadCommand final : public GdbCommand
{
public:
    virtual ~GdbRegisterReadCommand() override;
    virtual void Process(int socket) override;
};

class GdbMemoryReadCommand final : public GdbCommand
{
public:
    GdbMemoryReadCommand(uint64_t address, uint64_t length);
    virtual ~GdbMemoryReadCommand() override;
    virtual void Process(int socket) override;

private:
    uint64_t m_Address;
    uint64_t m_Length;
};

class GdbQuerySupportedCommand final : public GdbCommand
{
public:
    GdbQuerySupportedCommand(size_t packetSize);
    virtual ~GdbQuerySupportedCommand() override;
    virtual void Process(int socket) override;

private:
    size_t m_PacketSize;
};

}}
