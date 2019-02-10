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

#include <rvtrace/reader.h>

#include "TraceCycleReaderImpl.h"

namespace rvtrace {

TraceCycleReader::TraceCycleReader(const void* buffer, int64_t bufferSize)
{
    m_pImpl = new TraceCycleReaderImpl(buffer, bufferSize);
}

TraceCycleReader::~TraceCycleReader()
{
    delete m_pImpl;
}

const void* TraceCycleReader::GetNode(NodeType nodeType) const
{
    assert(GetNodeCount(nodeType) == 1);

    return GetNode(nodeType, 0);
}

const void* TraceCycleReader::GetNode(NodeType nodeType, int index) const
{
    return m_pImpl->GetNode(nodeType, index);
}

int64_t TraceCycleReader::GetNodeSize(NodeType nodeType) const
{
    assert(GetNodeCount(nodeType) == 1);

    return m_pImpl->GetNodeSize(nodeType, 0);
}

int64_t TraceCycleReader::GetNodeSize(NodeType nodeType, int index) const
{
    return m_pImpl->GetNodeSize(nodeType, index);
}

int TraceCycleReader::GetNodeCount(NodeType nodeType) const
{
    return m_pImpl->GetNodeCount(nodeType);
}

const BasicInfoNode* TraceCycleReader::GetBasicInfoNode() const
{
    return m_pImpl->GetBasicInfoNode();
}

const FpRegNode* TraceCycleReader::GetFpRegNode() const
{
    return m_pImpl->GetFpRegNode();
}

const IntReg32Node* TraceCycleReader::GetIntReg32Node() const
{
    return m_pImpl->GetIntReg32Node();
}

const IntReg64Node* TraceCycleReader::GetIntReg64Node() const
{
    return m_pImpl->GetIntReg64Node();
}

const Pc32Node* TraceCycleReader::GetPc32Node() const
{
    return m_pImpl->GetPc32Node();
}

const Pc64Node* TraceCycleReader::GetPc64Node() const
{
    return m_pImpl->GetPc64Node();
}

const Csr32Node* TraceCycleReader::GetCsr32Node() const
{
    return m_pImpl->GetCsr32Node();
}

const Csr64Node* TraceCycleReader::GetCsr64Node() const
{
    return m_pImpl->GetCsr64Node();
}

const Trap32Node* TraceCycleReader::GetTrap32Node() const
{
    return m_pImpl->GetTrap32Node();
}

const Trap64Node* TraceCycleReader::GetTrap64Node() const
{
    return m_pImpl->GetTrap64Node();
}

const MemoryAccessNode* TraceCycleReader::GetMemoryAccessNode(int index) const
{
    return m_pImpl->GetMemoryAccessNode(index);
}

const IoNode* TraceCycleReader::GetIoNode() const
{
    return m_pImpl->GetIoNode();
}

const void* TraceCycleReader::GetMemoryNode() const
{
    return m_pImpl->GetMemoryNode();
}

}
