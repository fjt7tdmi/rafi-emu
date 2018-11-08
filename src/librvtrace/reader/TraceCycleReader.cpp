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
    return m_pImpl->GetNode(nodeType);
}

int64_t TraceCycleReader::GetNodeSize(NodeType nodeType) const
{
    return m_pImpl->GetNodeSize(nodeType);
}

bool TraceCycleReader::IsNodeExist(NodeType nodeType) const
{
    return m_pImpl->IsNodeExist(nodeType);
}

const BasicInfoNode* TraceCycleReader::GetBasicInfoNode() const
{
    return m_pImpl->GetBasicInfoNode();
}

const Pc32Node* TraceCycleReader::GetPc32Node() const
{
    return m_pImpl->GetPc32Node();
}

const Pc64Node* TraceCycleReader::GetPc64Node() const
{
    return m_pImpl->GetPc64Node();
}

const IntReg32Node* TraceCycleReader::GetIntReg32Node() const
{
    return m_pImpl->GetIntReg32Node();
}

const IntReg64Node* TraceCycleReader::GetIntReg64Node() const
{
    return m_pImpl->GetIntReg64Node();
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

const MemoryAccess32Node* TraceCycleReader::GetMemoryAccess32Node() const
{
    return m_pImpl->GetMemoryAccess32Node();
}

const MemoryAccess64Node* TraceCycleReader::GetMemoryAccess64Node() const
{
    return m_pImpl->GetMemoryAccess64Node();
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
