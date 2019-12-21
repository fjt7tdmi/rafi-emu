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
#include <string>

#include <rafi/emu.h>

#include "../ISystem.h"

#include "GdbData.h"

namespace rafi { namespace emu {

class IGdbCommand
{
public:
    virtual ~IGdbCommand()
    {
    }

    virtual std::string Process(ISystem* pSystem, GdbData* pData) = 0;
};

class GdbCommandInvalid : public IGdbCommand
{
public:
    GdbCommandInvalid();

    std::string Process(ISystem* pSystem, GdbData* pData) override;
};

class GdbCommandReadReg : public IGdbCommand
{
public:
    GdbCommandReadReg(XLEN xlen);

    std::string Process(ISystem* pSystem, GdbData* pData) override;

private:
    std::string Process32(ISystem* pSystem);
    std::string Process64(ISystem* pSystem);

    XLEN m_XLEN;
};

class GdbCommandReadMemory : public IGdbCommand
{
public:
    GdbCommandReadMemory(const std::string& cmd);
    virtual ~GdbCommandReadMemory();

    std::string Process(ISystem* pSystem, GdbData* pData) override;

    paddr_t GetAddr() const;
    size_t GetSize() const;

private:
    paddr_t m_Addr;
    size_t m_Size;
    uint8_t* m_pBuffer;
};

class GdbCommandInsertBreakPoint : public IGdbCommand
{
public:
    GdbCommandInsertBreakPoint(const std::string& cmd);

    std::string Process(ISystem* pSystem, GdbData* pData) override;

    paddr_t GetAddr() const;
    size_t GetSize() const;

private:
    static const uint32_t BreakInsn = 0x100073; // ebreak
    static const uint16_t ShortBreakInsn = 0x9002; // c.ebreak

    paddr_t m_Addr;
    size_t m_Size;
};

class GdbCommandRemoveBreakPoint : public IGdbCommand
{
public:
    GdbCommandRemoveBreakPoint(const std::string& cmd);

    std::string Process(ISystem* pSystem, GdbData* pData) override;

    paddr_t GetAddr() const;
    size_t GetSize() const;

private:
    paddr_t m_Addr;
    size_t m_Size;
};

class GdbCommandContinue : public IGdbCommand
{
public:
    GdbCommandContinue();

    std::string Process(ISystem* pSystem, GdbData* pData) override;
};

class GdbCommandStep : public IGdbCommand
{
public:
    GdbCommandStep();

    std::string Process(ISystem* pSystem, GdbData* pData) override;
};

class GdbCommandSetThread : public IGdbCommand
{
public:
    GdbCommandSetThread();

    std::string Process(ISystem* pSystem, GdbData* pData) override;
};

class GdbCommandStopReason : public IGdbCommand
{
public:
    GdbCommandStopReason();

    std::string Process(ISystem* pSystem, GdbData* pData) override;
};

class GdbCommandQuery : public IGdbCommand
{
public:
    GdbCommandQuery(const std::string& cmd);

    std::string Process(ISystem* pSystem, GdbData* pData) override;

private:
    std::string m_Command;
};

class GdbCommandContinueQuery : public IGdbCommand
{
public:
    GdbCommandContinueQuery();

    std::string Process(ISystem* pSystem, GdbData* pData) override;
};

}}
