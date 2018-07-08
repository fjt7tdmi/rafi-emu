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

#include "../System/Bus/Bus.h"
#include "TraceBinary.h"
#include "TraceBinaryWriter.h"

#pragma warning (disable: 4996)

TraceBinaryWriter::TraceBinaryWriter(const char* path, const System* pSystem)
    : m_pSystem(pSystem)
{
    m_File = std::fopen(path, "wb");
    if (m_File == nullptr)
    {
        throw FileOpenFailureException(path);
    }

    TraceBinaryHeader header;
    for (int i = 0; i < 8; i++)
    {
        header.signature[i] = TraceBinarySignature[i];
    }
    header.headerSize = sizeof(TraceBinaryHeader);

    std::fwrite(&header, sizeof(TraceBinaryHeader), 1, m_File);
}

TraceBinaryWriter::~TraceBinaryWriter()
{
    // Write end node
    TraceHeader header = {0};

    std::fwrite(&header, sizeof(header), 1, m_File);
    std::fclose(m_File);
}

void TraceBinaryWriter::EnableDump()
{
    m_Enabled = true;
}

void TraceBinaryWriter::EnableDumpCsr()
{
    m_EnableDumpCsr = true;
}

void TraceBinaryWriter::EnableDumpMemory()
{
    m_EnableDumpMemory = true;
}

void TraceBinaryWriter::DumpOneCycle(int cycle)
{
    if (!m_Enabled)
    {
        return;
    }

    // TraceHeader
    int64_t nodeSize = sizeof(TraceHeader);
    nodeSize += sizeof(BasicInfoNode);
    nodeSize += sizeof(Pc32Node);
    nodeSize += sizeof(IntReg32Node);
    nodeSize += m_pSystem->IsTrapEventExist() ? sizeof(Trap32Node) : 0;
    nodeSize += m_pSystem->IsMemoryAccessEventExist() ? sizeof(MemoryAccess32Node) : 0;
    nodeSize += sizeof(IoNode);

    if (m_EnableDumpCsr)
    {
        nodeSize += sizeof(Csr32NodeHeader) + m_pSystem->GetCsrSize();
    }

    if (m_EnableDumpMemory)
    {
        nodeSize += sizeof(MemoryNodeHeader) + m_pSystem->GetMemorySize();
    }

    TraceHeader header = { nodeSize };

    std::fwrite(&header, sizeof(header), 1, m_File);

    // OpEvent
    assert(m_pSystem->IsOpEventExist());

    OpEvent opEvent;
    m_pSystem->CopyOpEvent(&opEvent);

    // BasicInfoNode
    BasicInfoNode basicInfoNode;

    basicInfoNode.header.nodeSize = sizeof(BasicInfoNode);
    basicInfoNode.header.nodeType = NodeType::BasicInfo;
    basicInfoNode.header.reserved = 0;
    basicInfoNode.cycle = cycle;
    basicInfoNode.opId = opEvent.opId;

    std::memset(&basicInfoNode.reserved, 0, sizeof(basicInfoNode.reserved));

    std::fwrite(&basicInfoNode, sizeof(basicInfoNode), 1, m_File);

    // Pc32Node
    Pc32Node pc32Node;

    pc32Node.header.nodeSize = sizeof(Pc32Node);
    pc32Node.header.nodeType = NodeType::Pc32;
    pc32Node.header.reserved = 0;
    pc32Node.physicalPc = static_cast<int32_t>(opEvent.physicalPc);
    pc32Node.virtualPc = opEvent.virtualPc;

    std::fwrite(&pc32Node, sizeof(pc32Node), 1, m_File);

    // IntReg32Node
    IntReg32Node intRegNode;
    m_pSystem->CopyIntRegs(&intRegNode.regs, sizeof(intRegNode.regs));

    intRegNode.header.nodeSize = sizeof(IntReg32Node);
    intRegNode.header.nodeType = NodeType::IntReg32;
    intRegNode.header.reserved = 0;

    std::fwrite(&intRegNode, sizeof(intRegNode), 1, m_File);

    // Trap32Node
    if (m_pSystem->IsTrapEventExist())
    {
        TrapEvent trapEvent;
        m_pSystem->CopyTrapEvent(&trapEvent);

        Trap32Node trap32Node;
        trap32Node.header.nodeSize = sizeof(Trap32Node);
        trap32Node.header.nodeType = NodeType::Trap32;
        trap32Node.header.reserved = 0;
        trap32Node.from = trapEvent.from;
        trap32Node.to = trapEvent.to;
        trap32Node.cause = static_cast<int32_t>(trapEvent.trapCause);
        trap32Node.trapType = trapEvent.trapType;
        trap32Node.trapValue = trapEvent.trapValue;
        trap32Node.reserved = 0;

        std::fwrite(&trap32Node, sizeof(trap32Node), 1, m_File);
    }

    // MemoryAccess32Node
    if (m_pSystem->IsMemoryAccessEventExist())
    {
        MemoryAccessEvent memoryAccessEvent;
        m_pSystem->CopyMemoryAccessEvent(&memoryAccessEvent);

        MemoryAccess32Node memoryAccessNode;
        memoryAccessNode.header.nodeSize = sizeof(MemoryAccess32Node);
        memoryAccessNode.header.nodeType = NodeType::MemoryAccess32;
        memoryAccessNode.header.reserved = 0;
        memoryAccessNode.memoryAccessSize = memoryAccessEvent.accessSize;
        memoryAccessNode.memoryAccessType = memoryAccessEvent.accessType;
        memoryAccessNode.physicalAddress = static_cast<int32_t>(memoryAccessEvent.physicalAddress);
        memoryAccessNode.virtualAddress = memoryAccessEvent.virtualAddress;
        memoryAccessNode.value = memoryAccessEvent.value;
        std::memset(&memoryAccessNode.reserved, 0, sizeof(memoryAccessNode.reserved));

        std::fwrite(&memoryAccessNode, sizeof(memoryAccessNode), 1, m_File);
    }

    // Csr32Node
    if (m_EnableDumpCsr)
    {
        const auto bodySize = m_pSystem->GetCsrSize();

        Csr32NodeHeader node;
        node.header.nodeSize = sizeof(Csr32NodeHeader) + bodySize;
        node.header.nodeType = NodeType::Csr32;
        node.header.reserved = 0;

        auto body = std::make_unique<char[]>(bodySize);
        m_pSystem->CopyCsr(body.get(), bodySize);

        std::fwrite(&node, sizeof(node), 1, m_File);
        std::fwrite(body.get(), bodySize, 1, m_File);
    }

    // Memory
    if (m_EnableDumpMemory)
    {
        const auto bodySize = m_pSystem->GetMemorySize();

        MemoryNodeHeader node;
        node.header.nodeSize = sizeof(MemoryNodeHeader) + bodySize;
        node.header.nodeType = NodeType::Memory;
        node.header.reserved = 0;

        auto body = std::make_unique<char[]>(bodySize);
        m_pSystem->CopyMemory(body.get(), bodySize);

        std::fwrite(&node, sizeof(node), 1, m_File);
        std::fwrite(body.get(), bodySize, 1, m_File);
    }

    // IoNode
    IoNode ioNode;
    ioNode.header.nodeSize = sizeof(IoNode);
    ioNode.header.nodeType = NodeType::Io;
    ioNode.header.reserved = 0;
    ioNode.hostIoValue = static_cast<int64_t>(m_pSystem->GetHostIoValue());

    std::fwrite(&ioNode, sizeof(ioNode), 1, m_File);

    std::fflush(m_File);
}
