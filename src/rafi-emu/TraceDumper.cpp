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

#include "System/Bus/Bus.h"
#include "TraceDumper.h"

#pragma warning (disable: 4996)

TraceDumper::TraceDumper(const char* path, const System* pSystem)
    : m_FileTraceWriter(path)
    , m_pSystem(pSystem)
{
}

TraceDumper::~TraceDumper()
{
}

void TraceDumper::EnableDump()
{
    m_Enabled = true;
}

void TraceDumper::EnableDumpCsr()
{
    m_EnableDumpCsr = true;
}

void TraceDumper::EnableDumpMemory()
{
    m_EnableDumpMemory = true;
}

void TraceDumper::DumpOneCycle(int cycle)
{
    if (!m_Enabled)
    {
        return;
    }

    // TraceHeader
    int32_t flags = NodeFlag_BasicInfo | NodeFlag_Pc32 | NodeFlag_IntReg32 | NodeFlag_Io;

    if (m_pSystem->IsTrapEventExist())
    {
        flags |= NodeFlag_Trap32;
    }
    
    if (m_pSystem->IsMemoryAccessEventExist())
    {
        flags |= NodeFlag_MemoryAccess32;
    }

    if (m_EnableDumpCsr)
    {
        flags |= NodeFlag_Csr32;
    }

    if (m_EnableDumpMemory)
    {
        flags |= NodeFlag_Memory;
    }

    TraceCycleBuilder builder(flags);

    builder.SetOffsetOfPreviousCycle(-m_FileTraceWriter.GetPreviousWriteSize());

    // OpEvent
    assert(m_pSystem->IsOpEventExist());

    OpEvent opEvent;
    m_pSystem->CopyOpEvent(&opEvent);

    // BasicInfoNode
    BasicInfoNode basicInfoNode
    {
        cycle,
        opEvent.opId,
        opEvent.insn,
        {0, 0, 0, 0},
    };
    builder.SetNode(basicInfoNode);

    // Pc32Node
    Pc32Node pc32Node
    {
        opEvent.virtualPc,
        static_cast<int32_t>(opEvent.physicalPc),
    };
    builder.SetNode(pc32Node);

    // IntReg32Node
    // TODO: optimize (values are double copied now)
    IntReg32Node intRegNode;    

    m_pSystem->CopyIntRegs(&intRegNode.regs, sizeof(intRegNode.regs));
    
    builder.SetNode(intRegNode);

    // Trap32Node
    // TODO: optimize (values are double copied now)
    if (m_pSystem->IsTrapEventExist())
    {
        TrapEvent trapEvent;
        m_pSystem->CopyTrapEvent(&trapEvent);

        Trap32Node trap32Node
        {
            trapEvent.trapType,
            trapEvent.from,
            trapEvent.to,
            static_cast<int32_t>(trapEvent.trapCause),
            trapEvent.trapValue,
            0,
        };
        builder.SetNode(trap32Node);
    }

    // MemoryAccess32Node
    if (m_pSystem->IsMemoryAccessEventExist())
    {
        MemoryAccessEvent memoryAccessEvent;
        m_pSystem->CopyMemoryAccessEvent(&memoryAccessEvent);

        MemoryAccess32Node memoryAccessNode
        {
            memoryAccessEvent.virtualAddress,
            static_cast<int32_t>(memoryAccessEvent.physicalAddress),
            memoryAccessEvent.value,
            memoryAccessEvent.accessType,
            memoryAccessEvent.accessSize,
            {0, 0},
        };
        builder.SetNode(memoryAccessNode);
    }

    // Csr32Node
    if (m_EnableDumpCsr)
    {
        throw NotImplementedException(__FILE__, __LINE__);
    }

    // Memory
    if (m_EnableDumpMemory)
    {
        throw NotImplementedException(__FILE__, __LINE__);
    }

    // IoNode
    IoNode ioNode
    {
        static_cast<int32_t>(m_pSystem->GetHostIoValue()),
        0,
    };
    builder.SetNode(ioNode);

    m_FileTraceWriter.Write(builder.GetData(), builder.GetDataSize());
}