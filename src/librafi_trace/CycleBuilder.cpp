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
#include <cstdlib>
#include <cstring>

#include <rafi/trace.h>

#include "CycleBuilderImpl.h"

namespace rafi { namespace trace {

CycleBuilder::CycleBuilder(const CycleConfig& config)
{
    m_pImpl = new CycleBuilderImpl(config);
}

CycleBuilder::~CycleBuilder()
{
    delete m_pImpl;
}

void* CycleBuilder::GetData()
{
    return m_pImpl->GetData();
}

int64_t CycleBuilder::GetDataSize()
{
    return m_pImpl->GetDataSize();
}

int64_t CycleBuilder::GetNodeSize(NodeType nodeType)
{
    return m_pImpl->GetNodeSize(nodeType);
}

void* CycleBuilder::GetPointerToNode(NodeType nodeType)
{
    return m_pImpl->GetPointerToNode(nodeType);
}

void CycleBuilder::SetNode(NodeType nodeType, int index, const void* buffer, int64_t bufferSize)
{
    m_pImpl->SetNode(nodeType, index, buffer, bufferSize);
}

void CycleBuilder::SetNode(const BasicInfoNode& node)
{
    m_pImpl->SetNode(node);
}

void CycleBuilder::SetNode(const FpRegNode& node)
{
    m_pImpl->SetNode(node);
}

void CycleBuilder::SetNode(const Pc32Node& node)
{
    m_pImpl->SetNode(node);
}

void CycleBuilder::SetNode(const Pc64Node& node)
{
    m_pImpl->SetNode(node);
}

void CycleBuilder::SetNode(const IntReg32Node& node)
{
    m_pImpl->SetNode(node);
}

void CycleBuilder::SetNode(const IntReg64Node& node)
{
    m_pImpl->SetNode(node);
}

void CycleBuilder::SetNode(const Trap32Node& node)
{
    m_pImpl->SetNode(node);
}

void CycleBuilder::SetNode(const Trap64Node& node)
{
    m_pImpl->SetNode(node);
}

void CycleBuilder::SetNode(const MemoryAccessNode& node, int index)
{
    m_pImpl->SetNode(node, index);
}

void CycleBuilder::SetNode(const IoNode& node)
{
    m_pImpl->SetNode(node);
}

}}
