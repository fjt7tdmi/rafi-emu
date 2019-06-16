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
    if (m_pTraceBinaryWriter != nullptr)
    {
        delete m_pTraceBinaryWriter;
        m_pTraceBinaryWriter = nullptr;
    }
}

void TraceLogger::EnableDump()
{
    if (m_Enabled)
    {
        return;
    }

    m_Enabled = true;
    m_pTraceBinaryWriter = new TraceBinaryWriter(m_pPath);
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

void TraceLogger::DumpCycle(int cycle)
{
    if (!m_Enabled)
    {
        return;
    }

    BinaryCycleLogger cycleLogger(cycle, m_XLEN, m_pSystem->GetPc());

    // IntReg
    if (m_EnableDumpIntReg)
    {
        if (m_XLEN == XLEN::XLEN32)
        {
            NodeIntReg32 node;
            m_pSystem->CopyIntReg(&node);

            cycleLogger.Add(node);
        }
        else if (m_XLEN == XLEN::XLEN64)
        {
            NodeIntReg64 node;
            m_pSystem->CopyIntReg(&node);

            cycleLogger.Add(node);            
        }
        else
        {
            RAFI_EMU_NOT_IMPLEMENTED();
        }
    }

    // FpReg
    if (m_EnableDumpFpReg)
    {
        NodeFpReg node;
        m_pSystem->CopyFpReg(&node, sizeof(node));

        cycleLogger.Add(node);
    }

    // Io
    if (m_EnableDumpHostIo)
    {
        NodeIo node
        {
            m_pSystem->GetHostIoValue(),
            0,
        };

        cycleLogger.Add(node);
    }

    // OpEvent
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

    // TrapEvent
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

    // MemoryAccessNode
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

    // Csr64Node
    if (m_EnableDumpCsr)
    {
        RAFI_NOT_IMPLEMENTED();
    }

    // Memory
    if (m_EnableDumpMemory)
    {
        RAFI_NOT_IMPLEMENTED();
    }

    cycleLogger.Break();
    m_pTraceBinaryWriter->Write(cycleLogger.GetData(), cycleLogger.GetDataSize());
}

}}
