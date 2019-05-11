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

#include "TraceDumper.h"

#pragma warning (disable: 4996)

using namespace rafi::trace;

namespace rafi { namespace emu {

TraceDumper::TraceDumper(XLEN xlen, const char* path, const System* pSystem)
    : m_FileTraceWriter(path)
    , m_pSystem(pSystem)
    , m_XLEN(xlen)
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

void TraceDumper::EnableDumpFpReg()
{
    m_EnableDumpFpReg = true;
}

void TraceDumper::EnableDumpIntReg()
{
    m_EnableDumpIntReg = true;
}

void TraceDumper::EnableDumpMemory()
{
    m_EnableDumpMemory = true;
}

void TraceDumper::EnableDumpHostIo()
{
    m_EnableDumpHostIo = true;
}

void TraceDumper::DumpCycle(int cycle)
{
    if (!m_Enabled)
    {
        return;
    }

    if (!m_pSystem->IsOpEventExist())
    {
        return;
    }

    switch (m_XLEN)
    {
    case XLEN::XLEN32:
        DumpCycle32(cycle);
        break;
    case XLEN::XLEN64:
        DumpCycle64(cycle);
        break;
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }

}

void TraceDumper::DumpCycle32(int cycle)
{
    // TraceHeader
    CycleConfig config;
    config.SetNodeCount(NodeType::BasicInfo, 1);
    config.SetNodeCount(NodeType::Pc32, 1);

    if (m_pSystem->IsTrapEventExist())
    {
        config.SetNodeCount(NodeType::Trap32, 1);
    }

    config.SetNodeCount(NodeType::MemoryAccess, static_cast<int>(m_pSystem->GetMemoryAccessEventCount()));

    if (m_EnableDumpCsr)
    {
        config.SetNodeCount(NodeType::Csr32, 1);
    }
    if (m_EnableDumpFpReg)
    {
        config.SetNodeCount(NodeType::FpReg, 1);
    }
    if (m_EnableDumpIntReg)
    {
        config.SetNodeCount(NodeType::IntReg32, 1);
    }
    if (m_EnableDumpMemory)
    {
        config.SetNodeCount(NodeType::Memory, 1);
    }
    if (m_EnableDumpHostIo)
    {
        config.SetNodeCount(NodeType::Io, 1);
    }

    config.SetCsrCount(m_pSystem->GetCsrCount());
    config.SetRamSize(static_cast<int>(m_pSystem->GetRamSize()));

    CycleBuilder builder(config);

    // OpEvent
    OpEvent opEvent;
    m_pSystem->CopyOpEvent(&opEvent);

    // BasicInfoNode
    BasicInfoNode basicInfoNode
    {
        static_cast<uint32_t>(cycle),
        opEvent.opId,
        opEvent.insn,
        opEvent.privilegeLevel,
    };
    builder.SetNode(basicInfoNode);

    // Pc32Node
    Pc32Node pc32Node
    {
        static_cast<uint32_t>(opEvent.virtualPc),
        static_cast<uint32_t>(opEvent.physicalPc),
    };
    builder.SetNode(pc32Node);

    // IntReg32Node
    if (m_EnableDumpIntReg)
    {
        IntReg32Node intRegNode;
        m_pSystem->CopyIntReg(&intRegNode);

        builder.SetNode(intRegNode);
    }

    // FpRegNode
    if (m_EnableDumpFpReg)
    {
        FpRegNode fpRegNode;
        m_pSystem->CopyFpReg(&fpRegNode, sizeof(fpRegNode));

        builder.SetNode(fpRegNode);
    }

    // Trap32Node
    if (m_pSystem->IsTrapEventExist())
    {
        TrapEvent trapEvent;
        m_pSystem->CopyTrapEvent(&trapEvent);

        Trap32Node trap32Node
        {
            trapEvent.trapType,
            trapEvent.from,
            trapEvent.to,
            trapEvent.trapCause,
            static_cast<uint32_t>(trapEvent.trapValue),
            0,
        };
        builder.SetNode(trap32Node);
    }

    // Csr32Node
    if (m_EnableDumpCsr)
    {
        auto pNode = reinterpret_cast<Csr32Node*>(builder.GetPointerToNode(NodeType::Csr32));
        auto nodeCount = static_cast<int>(builder.GetNodeSize(NodeType::Csr32) / sizeof(Csr32Node));

        m_pSystem->CopyCsr(pNode, nodeCount);
    }

    // MemoryAccessNode
    for (int index = 0; index < m_pSystem->GetMemoryAccessEventCount(); index++)
    {
        MemoryAccessEvent memoryAccessEvent;
        m_pSystem->CopyMemoryAccessEvent(&memoryAccessEvent, index);

        MemoryAccessNode memoryAccessNode
        {
            memoryAccessEvent.accessType,
            memoryAccessEvent.size,
            memoryAccessEvent.value,
            memoryAccessEvent.virtualAddress,
            memoryAccessEvent.physicalAddress,
        };
        builder.SetNode(memoryAccessNode, index);
    }

    // Memory
    if (m_EnableDumpMemory)
    {
        const auto size = static_cast<size_t>(builder.GetNodeSize(NodeType::Memory));
        auto buffer = builder.GetPointerToNode(NodeType::Memory);

        m_pSystem->CopyRam(buffer, size);
    }

    // IoNode
    if (m_EnableDumpHostIo)
    {
        IoNode ioNode
        {
            m_pSystem->GetHostIoValue(),
            0,
        };
        builder.SetNode(ioNode);
    }

    m_FileTraceWriter.Write(builder.GetData(), builder.GetDataSize());
}

void TraceDumper::DumpCycle64(int cycle)
{
    // TraceHeader
    CycleConfig config;
    config.SetNodeCount(NodeType::BasicInfo, 1);
    config.SetNodeCount(NodeType::Pc64, 1);

    if (m_pSystem->IsTrapEventExist())
    {
        config.SetNodeCount(NodeType::Trap64, 1);
    }

    config.SetNodeCount(NodeType::MemoryAccess, static_cast<int>(m_pSystem->GetMemoryAccessEventCount()));

    if (m_EnableDumpFpReg)
    {
        config.SetNodeCount(NodeType::FpReg, 1);
    }
    if (m_EnableDumpIntReg)
    {
        config.SetNodeCount(NodeType::IntReg64, 1);
    }
    if (m_EnableDumpCsr)
    {
        config.SetNodeCount(NodeType::Csr64, 1);
    }
    if (m_EnableDumpMemory)
    {
        config.SetNodeCount(NodeType::Memory, 1);
    }
    if (m_EnableDumpHostIo)
    {
        config.SetNodeCount(NodeType::Io, 1);
    }

    config.SetCsrCount(m_pSystem->GetCsrCount());
    config.SetRamSize(static_cast<int>(m_pSystem->GetRamSize()));

    CycleBuilder builder(config);

    // OpEvent
    OpEvent opEvent;
    m_pSystem->CopyOpEvent(&opEvent);

    // BasicInfoNode
    BasicInfoNode basicInfoNode
    {
        static_cast<uint32_t>(cycle),
        opEvent.opId,
        opEvent.insn,
        opEvent.privilegeLevel,
    };
    builder.SetNode(basicInfoNode);

    // Pc64Node
    Pc64Node pc64Node
    {
        opEvent.virtualPc,
        opEvent.physicalPc,
    };
    builder.SetNode(pc64Node);

    // IntReg64Node
    if (m_EnableDumpIntReg)
    {
        IntReg64Node intRegNode;
        m_pSystem->CopyIntReg(&intRegNode);

        builder.SetNode(intRegNode);
    }

    // FpRegNode
    if (m_EnableDumpFpReg)
    {
        FpRegNode fpRegNode;
        m_pSystem->CopyFpReg(&fpRegNode, sizeof(fpRegNode));

        builder.SetNode(fpRegNode);
    }

    // Trap64Node
    if (m_pSystem->IsTrapEventExist())
    {
        TrapEvent trapEvent;
        m_pSystem->CopyTrapEvent(&trapEvent);

        Trap64Node trap64Node
        {
            trapEvent.trapType,
            trapEvent.from,
            trapEvent.to,
            trapEvent.trapCause,
            trapEvent.trapValue,
        };
        builder.SetNode(trap64Node);
    }

    // Csr64Node
    if (m_EnableDumpCsr)
    {
        auto pNode = reinterpret_cast<Csr64Node*>(builder.GetPointerToNode(NodeType::Csr64));
        auto nodeCount = static_cast<int>(builder.GetNodeSize(NodeType::Csr64) / sizeof(Csr64Node));

        m_pSystem->CopyCsr(pNode, nodeCount);
    }

    // MemoryAccessNode
    for (int index = 0; index < m_pSystem->GetMemoryAccessEventCount(); index++)
    {
        MemoryAccessEvent memoryAccessEvent;
        m_pSystem->CopyMemoryAccessEvent(&memoryAccessEvent, index);

        MemoryAccessNode memoryAccessNode
        {
            memoryAccessEvent.accessType,
            memoryAccessEvent.size,
            memoryAccessEvent.value,
            memoryAccessEvent.virtualAddress,
            memoryAccessEvent.physicalAddress,
        };
        builder.SetNode(memoryAccessNode, index);
    }

    // Memory
    if (m_EnableDumpMemory)
    {
        const auto size = static_cast<size_t>(builder.GetNodeSize(NodeType::Memory));
        auto buffer = builder.GetPointerToNode(NodeType::Memory);

        m_pSystem->CopyRam(buffer, size);
    }

    // IoNode
    if (m_EnableDumpHostIo)
    {
        IoNode ioNode
        {
            m_pSystem->GetHostIoValue(),
            0,
        };
        builder.SetNode(ioNode);
    }

    m_FileTraceWriter.Write(builder.GetData(), builder.GetDataSize());
}

}}
