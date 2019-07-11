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

#include <memory>
#include <string>
#include <sstream>

#include <rafi/trace.h>

#include "bus/Bus.h"

#include "TraceLogger.h"

#pragma warning (disable: 4996)

using namespace rafi::trace;

namespace rafi { namespace emu {

TraceLogger::TraceLogger(XLEN xlen, const char* path, const System* pSystem)
    : m_XLEN(xlen)
    , m_pPath(path)
    , m_pSystem(pSystem)
{
}

TraceLogger::~TraceLogger()
{
    if (m_pTraceWriter != nullptr)
    {
        delete m_pTraceWriter;
        m_pTraceWriter = nullptr;
    }
}

void TraceLogger::EnableDump()
{
    if (m_Enabled)
    {
        return;
    }

    m_Enabled = true;
    m_pTraceWriter = new TraceIndexWriter(m_pPath);
}

void TraceLogger::EnableDumpCsr()
{
    m_EnableDumpCsr = true;
}

void TraceLogger::EnableDumpFpReg()
{
    m_EnableDumpFpReg = true;
}

void TraceLogger::EnableDumpIntReg()
{
    m_EnableDumpIntReg = true;
}

void TraceLogger::EnableDumpMemory()
{
    m_EnableDumpMemory = true;
}

void TraceLogger::EnableDumpHostIo()
{
    m_EnableDumpHostIo = true;
}

void TraceLogger::RecordState()
{
    // IntReg
    if (m_XLEN == XLEN::XLEN32)
    {
        m_pSystem->CopyIntReg(&m_IntReg32);
    }
    else if (m_XLEN == XLEN::XLEN64)
    {
        m_pSystem->CopyIntReg(&m_IntReg64);
    }
    else
    {
        RAFI_EMU_NOT_IMPLEMENTED();
    }

    // FpReg
    m_pSystem->CopyFpReg(&m_FpReg, sizeof(m_FpReg));

    // Io
    m_Io =
    {
        m_pSystem->GetHostIoValue(),
        0,
    };

    m_StateRecorded = true;
}

void TraceLogger::DumpCycle(int cycle)
{
    if (!m_Enabled)
    {
        return;
    }

    if (!m_StateRecorded)
    {
        RAFI_NOT_IMPLEMENTED();
    }
    m_StateRecorded = false;

    BinaryCycleLogger cycleLogger(cycle, m_XLEN, m_pSystem->GetPc());

    // State
    if (m_EnableDumpIntReg)
    {
        if (m_XLEN == XLEN::XLEN32)
        {
            cycleLogger.Add(m_IntReg32);
        }
        else if (m_XLEN == XLEN::XLEN64)
        {
            cycleLogger.Add(m_IntReg64);
        }
        else
        {
            RAFI_EMU_NOT_IMPLEMENTED();
        }
    }

    if (m_EnableDumpFpReg)
    {
        cycleLogger.Add(m_FpReg);
    }

    if (m_EnableDumpHostIo)
    {
        cycleLogger.Add(m_Io);
    }

    if (m_EnableDumpCsr)
    {
        RAFI_NOT_IMPLEMENTED();
    }

    if (m_EnableDumpMemory)
    {
        RAFI_NOT_IMPLEMENTED();
    }

    // Event
    if (m_pSystem->IsOpEventExist())
    {
        OpEvent opEvent;
        m_pSystem->CopyOpEvent(&opEvent);

        NodeOpEvent node
        {
            opEvent.insn,
            opEvent.privilegeLevel,
        };
        cycleLogger.Add(node);
    }

    if (m_pSystem->IsTrapEventExist())
    {
        TrapEvent trapEvent;
        m_pSystem->CopyTrapEvent(&trapEvent);

        NodeTrapEvent node
        {
            trapEvent.trapType,
            trapEvent.from,
            trapEvent.to,
            trapEvent.trapCause,
            trapEvent.trapValue,
        };

        cycleLogger.Add(node);
    }

    for (int index = 0; index < m_pSystem->GetMemoryAccessEventCount(); index++)
    {
        MemoryAccessEvent memoryAccessEvent;
        m_pSystem->CopyMemoryAccessEvent(&memoryAccessEvent, index);

        NodeMemoryEvent node
        {
            memoryAccessEvent.accessType,
            memoryAccessEvent.size,
            memoryAccessEvent.value,
            memoryAccessEvent.virtualAddress,
            memoryAccessEvent.physicalAddress,
        };
        cycleLogger.Add(node);
    }

    cycleLogger.Break();
    m_pTraceWriter->Write(cycleLogger.GetData(), cycleLogger.GetDataSize());
}

}}
