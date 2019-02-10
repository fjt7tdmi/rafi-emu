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

#include "TraceCycleBuilderImpl.h"

namespace rafi { namespace trace {

TraceCycleBuilder::TraceCycleBuilder(const TraceCycleConfig& config)
{
    m_pImpl = new TraceCycleBuilderImpl(config);
}

TraceCycleBuilder::~TraceCycleBuilder()
{
    delete m_pImpl;
}

void* TraceCycleBuilder::GetData()
{
    return m_pImpl->GetData();
}

int64_t TraceCycleBuilder::GetDataSize()
{
    return m_pImpl->GetDataSize();
}

int64_t TraceCycleBuilder::GetNodeSize(NodeType nodeType)
{
    return m_pImpl->GetNodeSize(nodeType);
}

void* TraceCycleBuilder::GetPointerToNode(NodeType nodeType)
{
    return m_pImpl->GetPointerToNode(nodeType);
}

void TraceCycleBuilder::SetNode(NodeType nodeType, int index, const void* buffer, int64_t bufferSize)
{
    m_pImpl->SetNode(nodeType, index, buffer, bufferSize);
}

void TraceCycleBuilder::SetNode(const BasicInfoNode& node)
{
    m_pImpl->SetNode(node);
}

void TraceCycleBuilder::SetNode(const FpRegNode& node)
{
    m_pImpl->SetNode(node);
}

void TraceCycleBuilder::SetNode(const Pc32Node& node)
{
    m_pImpl->SetNode(node);
}

void TraceCycleBuilder::SetNode(const Pc64Node& node)
{
    m_pImpl->SetNode(node);
}

void TraceCycleBuilder::SetNode(const IntReg32Node& node)
{
    m_pImpl->SetNode(node);
}

void TraceCycleBuilder::SetNode(const IntReg64Node& node)
{
    m_pImpl->SetNode(node);
}

void TraceCycleBuilder::SetNode(const Trap32Node& node)
{
    m_pImpl->SetNode(node);
}

void TraceCycleBuilder::SetNode(const Trap64Node& node)
{
    m_pImpl->SetNode(node);
}

void TraceCycleBuilder::SetNode(const MemoryAccessNode& node, int index)
{
    m_pImpl->SetNode(node, index);
}

void TraceCycleBuilder::SetNode(const IoNode& node)
{
    m_pImpl->SetNode(node);
}

}}
