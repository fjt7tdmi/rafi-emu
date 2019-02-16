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

#include "../bus/Bus.h"

#include "TraceDumper.h"

#pragma warning (disable: 4996)

using namespace rafi::trace;

namespace rafi { namespace emu {

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

    if (!m_pSystem->IsOpEventExist())
    {
        return;
    }

    // TraceHeader
    CycleConfig config;
    config.SetNodeCount(NodeType::BasicInfo, 1);
    config.SetNodeCount(NodeType::Pc32, 1);
    config.SetNodeCount(NodeType::IntReg32, 1);
    config.SetNodeCount(NodeType::FpReg, 1);
    config.SetNodeCount(NodeType::Io, 1);

    if (m_pSystem->IsTrapEventExist())
    {
        config.SetNodeCount(NodeType::Trap32, 1);
    }

    config.SetNodeCount(NodeType::MemoryAccess, m_pSystem->GetMemoryAccessEventCount());

    if (m_EnableDumpCsr)
    {
        config.SetNodeCount(NodeType::Csr32, 1);
    }

    if (m_EnableDumpMemory)
    {
        config.SetNodeCount(NodeType::Memory, 1);
    }

    config.SetCsrCount(m_pSystem->GetCsrCount());
    config.SetRamSize(m_pSystem->GetRamSize());

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
        opEvent.virtualPc,
        static_cast<uint32_t>(opEvent.physicalPc),
    };
    builder.SetNode(pc32Node);

    // IntReg32Node
    // TODO: optimize (values are double copied now)
    IntReg32Node intRegNode;

    m_pSystem->CopyIntReg(&intRegNode, sizeof(intRegNode));

    builder.SetNode(intRegNode);

    // FpRegNode
    // TODO: optimize (values are double copied now)
    FpRegNode fpRegNode;

    m_pSystem->CopyFpReg(&fpRegNode, sizeof(fpRegNode));

    builder.SetNode(fpRegNode);

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
            trapEvent.trapValue,
            0,
        };
        builder.SetNode(trap32Node);
    }

    // Csr32Node
    if (m_EnableDumpCsr)
    {
        const auto size = static_cast<size_t>(builder.GetNodeSize(NodeType::Csr32));
        auto buffer = builder.GetPointerToNode(NodeType::Csr32);

        m_pSystem->CopyCsr(buffer, size);
    }

    // MemoryAccessNode
    // TODO: optimize (values are double copied now)
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
    IoNode ioNode
    {
        m_pSystem->GetHostIoValue(),
        0,
    };
    builder.SetNode(ioNode);

    m_FileTraceWriter.Write(builder.GetData(), builder.GetDataSize());
}

}}
