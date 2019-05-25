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

#include "CycleViewImpl.h"

namespace rafi { namespace trace {

CycleView::CycleView(const void* buffer, int64_t bufferSize)
{
    m_pImpl = new CycleViewImpl(buffer, bufferSize);
}

CycleView::~CycleView()
{
    delete m_pImpl;
}

const void* CycleView::GetNode(NodeType nodeType) const
{
    assert(GetNodeCount(nodeType) == 1);

    return GetNode(nodeType, 0);
}

const void* CycleView::GetNode(NodeType nodeType, int index) const
{
    return m_pImpl->GetNode(nodeType, index);
}

int64_t CycleView::GetNodeSize(NodeType nodeType) const
{
    assert(GetNodeCount(nodeType) == 1);

    return m_pImpl->GetNodeSize(nodeType, 0);
}

int64_t CycleView::GetNodeSize(NodeType nodeType, int index) const
{
    return m_pImpl->GetNodeSize(nodeType, index);
}

int CycleView::GetNodeCount(NodeType nodeType) const
{
    return m_pImpl->GetNodeCount(nodeType);
}

const BasicInfoNode* CycleView::GetBasicInfoNode() const
{
    return m_pImpl->GetBasicInfoNode();
}

const FpRegNode* CycleView::GetFpRegNode() const
{
    return m_pImpl->GetFpRegNode();
}

const IntReg32Node* CycleView::GetIntReg32Node() const
{
    return m_pImpl->GetIntReg32Node();
}

const IntReg64Node* CycleView::GetIntReg64Node() const
{
    return m_pImpl->GetIntReg64Node();
}

const Pc32Node* CycleView::GetPc32Node() const
{
    return m_pImpl->GetPc32Node();
}

const Pc64Node* CycleView::GetPc64Node() const
{
    return m_pImpl->GetPc64Node();
}

const Csr32Node* CycleView::GetCsr32Node() const
{
    return m_pImpl->GetCsr32Node();
}

const Csr64Node* CycleView::GetCsr64Node() const
{
    return m_pImpl->GetCsr64Node();
}

const Trap32Node* CycleView::GetTrap32Node() const
{
    return m_pImpl->GetTrap32Node();
}

const Trap64Node* CycleView::GetTrap64Node() const
{
    return m_pImpl->GetTrap64Node();
}

const MemoryAccessNode* CycleView::GetMemoryAccessNode(int index) const
{
    return m_pImpl->GetMemoryAccessNode(index);
}

const IoNode* CycleView::GetIoNode() const
{
    return m_pImpl->GetIoNode();
}

const void* CycleView::GetMemoryNode() const
{
    return m_pImpl->GetMemoryNode();
}

}}
