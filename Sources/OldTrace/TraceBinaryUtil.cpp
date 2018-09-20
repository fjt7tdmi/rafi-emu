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

#include <algorithm>
#include <iostream>

#include "../Common/Exception.h"

#include "TraceBinary.h"
#include "TraceBinaryReader.h"

TraceChildHeader* FindTraceChild(NodeType nodeType, char* pNode, size_t nodeSize)
{
    int64_t offset = sizeof(TraceHeader);
    while (offset < nodeSize)
    {
        auto header = reinterpret_cast<TraceChildHeader*>(&pNode[offset]);
        if (header->nodeType == nodeType)
        {
            return reinterpret_cast<TraceChildHeader*>(header);
        }
        offset += header->nodeSize;
    }

    return nullptr;
}

BasicInfoNode* FindBasicInfoNode(char* pNode, size_t nodeSize)
{
    auto p = FindTraceChild(NodeType::BasicInfo, pNode, nodeSize);
    if (p == nullptr)
    {
        return nullptr;
    }

    if (p->nodeSize != sizeof(BasicInfoNode))
    {
        std::cout << "Detect invalid size BasicInfoNode (" << p->nodeSize << " byte)" << std::endl;
        return nullptr;
    }

    return reinterpret_cast<BasicInfoNode*>(pNode);
}

IoNode* FindIoNode(char* pNode, size_t nodeSize)
{
    auto p = FindTraceChild(NodeType::Io, pNode, nodeSize);
    if (p == nullptr)
    {
        return nullptr;
    }

    if (p->nodeSize != sizeof(IoNode))
    {
        std::cout << "Detect invalid size IoNode (" << p->nodeSize << " byte)" << std::endl;
        return nullptr;
    }

    return reinterpret_cast<IoNode*>(p);
}

Pc32Node* FindPc32Node(char* pNode, size_t nodeSize)
{
    auto p = FindTraceChild(NodeType::Pc32, pNode, nodeSize);
    if (p == nullptr)
    {
        return nullptr;
    }

    if (p->nodeSize != sizeof(Pc32Node))
    {
        std::cout << "Detect invalid size Pc32Node (" << p->nodeSize << " byte)" << std::endl;
        return nullptr;
    }

    return reinterpret_cast<Pc32Node*>(p);
}

IntReg32Node* FindIntReg32Node(char* pNode, size_t nodeSize)
{
    auto p = FindTraceChild(NodeType::IntReg32, pNode, nodeSize);
    if (p == nullptr)
    {
        return nullptr;
    }

    if (p->nodeSize != sizeof(IntReg32Node))
    {
        std::cout << "Detect invalid size IntReg32Node (" << p->nodeSize << " byte)" << std::endl;
        return nullptr;
    }

    return reinterpret_cast<IntReg32Node*>(p);
}

Csr32NodeHeader* FindCsr32Node(char* pNode, size_t nodeSize)
{
    auto p = FindTraceChild(NodeType::Csr32, pNode, nodeSize);
    if (p == nullptr)
    {
        return nullptr;
    }

    if (p->nodeSize < sizeof(Csr32NodeHeader))
    {
        std::cout << "Detect invalid size Csr32Node (" << p->nodeSize << " byte)" << std::endl;
        return nullptr;
    }

    auto pCsr32Node = reinterpret_cast<Csr32NodeHeader*>(p);
    if (p->nodeSize != sizeof(Csr32NodeHeader) + pCsr32Node->bodySize)
    {
        std::cout << "Detect invalid size MemoryNode (" << p->nodeSize << " byte)" << std::endl;
        return nullptr;
    }

    return pCsr32Node;
}

Trap32Node* FindTrap32Node(char* pNode, size_t nodeSize)
{
    auto p = FindTraceChild(NodeType::Trap32, pNode, nodeSize);
    if (p == nullptr)
    {
        return nullptr;
    }

    if (p->nodeSize != sizeof(Trap32Node))
    {
        std::cout << "Detect invalid size Trap32Node (" << p->nodeSize << " byte)" << std::endl;
        return nullptr;
    }

    return reinterpret_cast<Trap32Node*>(p);
}

MemoryAccess32Node* FindMemoryAccess32Node(char* pNode, size_t nodeSize)
{
    auto p = FindTraceChild(NodeType::MemoryAccess32, pNode, nodeSize);
    if (p == nullptr)
    {
        return nullptr;
    }

    if (p->nodeSize != sizeof(MemoryAccess32Node))
    {
        std::cout << "Detect invalid size MemoryAccess32Node (" << p->nodeSize << " byte)" << std::endl;
        return nullptr;
    }

    return reinterpret_cast<MemoryAccess32Node*>(p);
}

MemoryNodeHeader* FindMemoryNode(char* pNode, size_t nodeSize)
{
    auto p = FindTraceChild(NodeType::Memory, pNode, nodeSize);
    if (p == nullptr)
    {
        return nullptr;
    }

    if (p->nodeSize > sizeof(MemoryNodeHeader))
    {
        std::cout << "Detect invalid size MemoryNode (" << p->nodeSize << " byte)" << std::endl;
        return nullptr;
    }

    auto pMemoryNode = reinterpret_cast<MemoryNodeHeader*>(p);
    if (p->nodeSize != sizeof(MemoryNodeHeader) + pMemoryNode->bodySize)
    {
        std::cout << "Detect invalid size MemoryNode (" << p->nodeSize << " byte)" << std::endl;
        return nullptr;
    }

    return pMemoryNode;
}
