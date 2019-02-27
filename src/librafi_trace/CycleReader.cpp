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

#include <cassert>
#include <cstddef>
#include <cstdint>

#include <rafi/trace.h>

#include "CycleReaderImpl.h"

namespace rafi { namespace trace {

CycleReader::CycleReader(const void* buffer, int64_t bufferSize)
{
    m_pImpl = new CycleReaderImpl(buffer, bufferSize);
}

CycleReader::~CycleReader()
{
    delete m_pImpl;
}

const void* CycleReader::GetNode(NodeType nodeType) const
{
    assert(GetNodeCount(nodeType) == 1);

    return GetNode(nodeType, 0);
}

const void* CycleReader::GetNode(NodeType nodeType, int index) const
{
    return m_pImpl->GetNode(nodeType, index);
}

int64_t CycleReader::GetNodeSize(NodeType nodeType) const
{
    assert(GetNodeCount(nodeType) == 1);

    return m_pImpl->GetNodeSize(nodeType, 0);
}

int64_t CycleReader::GetNodeSize(NodeType nodeType, int index) const
{
    return m_pImpl->GetNodeSize(nodeType, index);
}

int CycleReader::GetNodeCount(NodeType nodeType) const
{
    return m_pImpl->GetNodeCount(nodeType);
}

const BasicInfoNode* CycleReader::GetBasicInfoNode() const
{
    return m_pImpl->GetBasicInfoNode();
}

const FpRegNode* CycleReader::GetFpRegNode() const
{
    return m_pImpl->GetFpRegNode();
}

const IntReg32Node* CycleReader::GetIntReg32Node() const
{
    return m_pImpl->GetIntReg32Node();
}

const IntReg64Node* CycleReader::GetIntReg64Node() const
{
    return m_pImpl->GetIntReg64Node();
}

const Pc32Node* CycleReader::GetPc32Node() const
{
    return m_pImpl->GetPc32Node();
}

const Pc64Node* CycleReader::GetPc64Node() const
{
    return m_pImpl->GetPc64Node();
}

const Csr32Node* CycleReader::GetCsr32Node() const
{
    return m_pImpl->GetCsr32Node();
}

const Csr64Node* CycleReader::GetCsr64Node() const
{
    return m_pImpl->GetCsr64Node();
}

const Trap32Node* CycleReader::GetTrap32Node() const
{
    return m_pImpl->GetTrap32Node();
}

const Trap64Node* CycleReader::GetTrap64Node() const
{
    return m_pImpl->GetTrap64Node();
}

const MemoryAccessNode* CycleReader::GetMemoryAccessNode(int index) const
{
    return m_pImpl->GetMemoryAccessNode(index);
}

const IoNode* CycleReader::GetIoNode() const
{
    return m_pImpl->GetIoNode();
}

const void* CycleReader::GetMemoryNode() const
{
    return m_pImpl->GetMemoryNode();
}

}}