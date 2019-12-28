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

#include <rafi/emu.h>

#include "GdbCommands.h"
#include "GdbException.h"
#include "GdbUtil.h"
#include "GdbTypes.h"

namespace rafi { namespace emu {

// ----------------------------------------------------------------------------

GdbCommandInvalid::GdbCommandInvalid()
{
}

std::string GdbCommandInvalid::Process(IEmulator*, GdbData*)
{
    return "";
}

// ----------------------------------------------------------------------------

GdbCommandReadReg::GdbCommandReadReg(XLEN xlen)
    : m_XLEN(xlen)
{
}

std::string GdbCommandReadReg::Process(IEmulator* pEmulator, GdbData*)
{
    switch (m_XLEN)
    {
    case XLEN::XLEN32:
        return Process32(pEmulator);
    case XLEN::XLEN64:
        return Process64(pEmulator);
    default:
        RAFI_EMU_NOT_IMPLEMENTED;
    }
}

std::string GdbCommandReadReg::Process32(IEmulator* pEmulator)
{
    std::string response;

    // IntReg
    trace::NodeIntReg32 intReg;
    pEmulator->CopyIntReg(&intReg);

    for (auto value: intReg.regs)
    {
        response += BinaryToHex(value);
    }

    // PC
    response += BinaryToHex(static_cast<uint32_t>(pEmulator->GetPc()));

    return response;
}

std::string GdbCommandReadReg::Process64(IEmulator* pEmulator)
{
    std::string response;

    // IntReg
    trace::NodeIntReg64 intReg;
    pEmulator->CopyIntReg(&intReg);

    for (auto value: intReg.regs)
    {
        response += BinaryToHex(value);
    }

    // PC
    response += BinaryToHex(static_cast<uint64_t>(pEmulator->GetPc()));

    return response;
}

// ----------------------------------------------------------------------------

GdbCommandReadMemory::GdbCommandReadMemory(const std::string& cmd)
{
    const auto comma = cmd.find(',');
    if (comma == std::string::npos)
    {
        RAFI_NOT_IMPLEMENTED;
    }

    const auto addr = cmd.substr(1, comma - 1);
    const auto size = cmd.substr(comma + 1);

    m_Addr = static_cast<paddr_t>(HexToUInt64(addr));
    m_Size = static_cast<size_t>(HexToUInt64(size));

    m_pBuffer = reinterpret_cast<uint8_t*>(malloc(m_Size));
}

GdbCommandReadMemory::~GdbCommandReadMemory()
{
    free(m_pBuffer);
}

std::string GdbCommandReadMemory::Process(IEmulator* pEmulator, GdbData*)
{
    std::string response;

    try
    {
        if (pEmulator->IsValidMemory(m_Addr, m_Size))
        {
            pEmulator->ReadMemory(m_pBuffer, m_Size, m_Addr);

            for (size_t i = 0; i < m_Size; i++)
            {
                response += BinaryToHex(m_pBuffer[i]);
            }
        }
        else
        {
            for (size_t i = 0; i < m_Size; i++)
            {
                response += "cd";
            }
        }

        return response;
    }
    catch (const RafiEmuException&)
    {
        return "E08"; // MEMORY_ERROR
    }
}

paddr_t GdbCommandReadMemory::GetAddr() const
{
    return m_Addr;
}

size_t GdbCommandReadMemory::GetSize() const
{
    return m_Size;
}

// ----------------------------------------------------------------------------

GdbCommandInsertBreakPoint::GdbCommandInsertBreakPoint(const std::string& cmd)
{
    const auto comma0 = cmd.find(',');
    const auto comma1 = cmd.find(',', comma0 + 1);
    if (comma0 == std::string::npos || comma1 == std::string::npos)
    {
        RAFI_NOT_IMPLEMENTED;
    }

    const auto addr = cmd.substr(comma0 + 1, comma1 - (comma0 + 1));
    const auto size = cmd.substr(comma1 + 1);

    m_Addr = HexToUInt64(addr);
    m_Size = HexToUInt64(size);
}

std::string GdbCommandInsertBreakPoint::Process(IEmulator* pEmulator, GdbData* pData)
{
    switch (m_Size)
    {
    case 2:
        {
            uint16_t value;
            pEmulator->ReadMemory(&value, sizeof(value), m_Addr);
            pData->PushMemoryValue(m_Addr, value);

            uint16_t insn = ShortBreakInsn;
            pEmulator->WriteMemory(&insn, sizeof(insn), m_Addr);
            return "OK";
        }
        break;
    case 4:
        {
            uint32_t value;
            pEmulator->ReadMemory(&value, sizeof(value), m_Addr);
            pData->PushMemoryValue(m_Addr, value);

            uint32_t insn = BreakInsn;
            pEmulator->WriteMemory(&insn, sizeof(insn), m_Addr);
            return "OK";
        }
        break;
    default:
        RAFI_NOT_IMPLEMENTED;
        break;
    }
}

paddr_t GdbCommandInsertBreakPoint::GetAddr() const
{
    return m_Addr;
}

size_t GdbCommandInsertBreakPoint::GetSize() const
{
    return m_Size;
}

// ----------------------------------------------------------------------------

GdbCommandRemoveBreakPoint::GdbCommandRemoveBreakPoint(const std::string& cmd)
{
    const auto comma0 = cmd.find(',');
    const auto comma1 = cmd.find(',', comma0 + 1);
    if (comma0 == std::string::npos || comma1 == std::string::npos)
    {
        RAFI_NOT_IMPLEMENTED;
    }

    const auto addr = cmd.substr(comma0 + 1, comma1 - (comma0 + 1));
    const auto size = cmd.substr(comma1 + 1);

    m_Addr = HexToUInt64(addr);
    m_Size = HexToUInt64(size);
}

std::string GdbCommandRemoveBreakPoint::Process(IEmulator* pEmulator, GdbData* pData)
{
    switch (m_Size)
    {
    case 2:
        {
            auto value = static_cast<uint16_t>(pData->PopMemoryValue(m_Addr));
            pEmulator->WriteMemory(&value, sizeof(value), m_Addr);
            return "OK";
        }
        break;
    case 4:
        {
            auto value = static_cast<uint32_t>(pData->PopMemoryValue(m_Addr));
            pEmulator->WriteMemory(&value, sizeof(value), m_Addr);
            return "OK";
        }
        break;
    default:
        RAFI_NOT_IMPLEMENTED;
        break;
    }
}

paddr_t GdbCommandRemoveBreakPoint::GetAddr() const
{
    return m_Addr;
}

size_t GdbCommandRemoveBreakPoint::GetSize() const
{
    return m_Size;
}

// ----------------------------------------------------------------------------

GdbCommandStep::GdbCommandStep()
{
}

std::string GdbCommandStep::Process(IEmulator* pEmulator, GdbData*)
{
    pEmulator->ProcessCycle();
    return "T05"; // 05: SIGTRAP
}

// ----------------------------------------------------------------------------

GdbCommandContinue::GdbCommandContinue()
{
}

std::string GdbCommandContinue::Process(IEmulator* pEmulator, GdbData*)
{
    pEmulator->Process(EmulationStop_Breakpoint);
    return "S05";
}

// ----------------------------------------------------------------------------

GdbCommandSetThread::GdbCommandSetThread()
{
    // TODO: support 'H' command completely
}

std::string GdbCommandSetThread::Process(IEmulator*, GdbData*)
{
    // This emulator supports only thread 0, but always returns 'OK' for H command.
    return "OK";
}

// ----------------------------------------------------------------------------

GdbCommandStopReason::GdbCommandStopReason()
{
    // TODO: support '?' command completely
}

std::string GdbCommandStopReason::Process(IEmulator*, GdbData*)
{
    return "S05"; // 05: SIGTRAP
}

// ----------------------------------------------------------------------------

GdbCommandQuery::GdbCommandQuery(const std::string& cmd)
    : m_Command(cmd)
{
}

std::string GdbCommandQuery::Process(IEmulator*, GdbData*)
{
    {
        auto pos = m_Command.find(';');
        auto name = m_Command.substr(0, pos);

        if (name == "qThreadExtraInfo")
        {
            return StringToHex("Breaked.");
        }
    }

    {
        auto pos = m_Command.find(':');
        auto name = m_Command.substr(0, pos);

        if (name == "qSupported")
        {
            char response[20] = {0};
            sprintf(response, "PacketSize=%zx", GdbCommandBufferSize);
            return response;
        }
        else if (name == "qfThreadInfo")
        {
            return "mp01.01"; // pid 1, tid 1
        }
        else if (name == "qsThreadInfo")
        {
            return "l"; // End of thread list
        }
        else if (name == "qC")
        {
            return "QCp01.01"; // pid 1, tid 1
        }
        else if (name == "qAttached")
        {
            return "1";
        }
        else
        {
            return "";
        }
    }
}

// ----------------------------------------------------------------------------

GdbCommandContinueQuery::GdbCommandContinueQuery()
{
}

std::string GdbCommandContinueQuery::Process(IEmulator*, GdbData*)
{
    return "vCont;c;s";
}

}}
