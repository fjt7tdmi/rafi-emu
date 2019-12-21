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

TraceLogger::TraceLogger(XLEN xlen, const TraceLoggerConfig& config, const System* pSystem)
    : m_XLEN(xlen)
    , m_Config(config)
    , m_pSystem(pSystem)
{
    if (m_Config.enabled)
    {
        m_pTraceWriter = new TraceIndexWriter(m_Config.path.c_str());
    }
}

TraceLogger::~TraceLogger()
{
    if (m_pTraceWriter != nullptr)
    {
        delete m_pTraceWriter;
    }
}

void TraceLogger::BeginCycle(int cycle, vaddr_t pc)
{
    if (!m_Config.enabled)
    {
        return;
    }

    m_pCurrentCycle = new BinaryCycleLogger(cycle, m_XLEN, pc);
}

void TraceLogger::RecordState()
{
    if (!m_Config.enabled)
    {
        return;
    }

    if (m_Config.enableDumpIntReg)
    {
        if (m_XLEN == XLEN::XLEN32)
        {
            NodeIntReg32 node;
            m_pSystem->CopyIntReg(&node);
            m_pCurrentCycle->Add(node);
        }
        else if (m_XLEN == XLEN::XLEN64)
        {
            NodeIntReg64 node;
            m_pSystem->CopyIntReg(&node);
            m_pCurrentCycle->Add(node);
        }
        else
        {
            RAFI_EMU_NOT_IMPLEMENTED;
        }
    }

    if (m_Config.enableDumpFpReg)
    {
        NodeFpReg node;
        m_pSystem->CopyFpReg(&node, sizeof(node));
        m_pCurrentCycle->Add(node);
    }

    if (m_Config.enableDumpHostIo)
    {
        NodeIo node = { m_pSystem->GetHostIoValue(), 0 };
        m_pCurrentCycle->Add(node);
    }

    if (m_Config.enableDumpCsr || m_Config.enableDumpMemory)
    {
        RAFI_NOT_IMPLEMENTED;
    }
}

void TraceLogger::RecordEvent()
{
    if (!m_Config.enabled)
    {
        return;
    }

    if (m_pSystem->IsOpEventExist())
    {
        OpEvent opEvent;
        m_pSystem->CopyOpEvent(&opEvent);

        NodeOpEvent node
        {
            opEvent.insn,
            opEvent.privilegeLevel,
        };
        m_pCurrentCycle->Add(node);
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

        m_pCurrentCycle->Add(node);
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
        m_pCurrentCycle->Add(node);
    }

}

void TraceLogger::EndCycle()
{
    if (!m_Config.enabled)
    {
        return;
    }

    m_pCurrentCycle->Break();

    m_pTraceWriter->Write(m_pCurrentCycle->GetData(), m_pCurrentCycle->GetDataSize());

    delete m_pCurrentCycle;
    m_pCurrentCycle = nullptr;
}

}}
