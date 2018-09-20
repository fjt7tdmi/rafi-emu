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

#include "TraceCycleReader.h"
#include "TraceCycleException.h"

TraceCycleReader::TraceCycleReader(const void* buffer, int64_t bufferSize)
{
    m_pData = buffer;
    m_BufferSize = bufferSize;
}

int64_t TraceCycleReader::GetOffsetOfPreviousCycle()
{
    return GetPointerToHeader()->prev;
}

int64_t TraceCycleReader::GetOffsetOfNextCycle()
{
    return GetPointerToHeader()->next;
}

const void* TraceCycleReader::GetNode(NodeType nodeType)
{
    auto pNode = GetPointerToNode(nodeType);

    if (pNode == nullptr)
    {
        throw TraceCycleException("Cannot find node.");
    }

    return pNode;
}

int64_t TraceCycleReader::GetNodeSize(NodeType nodeType)
{
    auto pMeta = GetPointerToMeta(nodeType);

    if (pMeta == nullptr)
    {
        throw TraceCycleException("Cannot find node.");
    }

    return pMeta->size;
}

bool TraceCycleReader::IsNodeExist(NodeType nodeType)
{
    return GetPointerToMeta(nodeType) != nullptr;
}

const TraceCycleHeader* TraceCycleReader::GetPointerToHeader()
{
    return reinterpret_cast<const TraceCycleHeader*>(m_pData);
}

const TraceCycleMetaNode* TraceCycleReader::GetPointerToMeta(int32_t index)
{
    assert(0 <= index);
    assert(index < GetPointerToHeader()->metaCount);

    auto metaNodes = reinterpret_cast<const TraceCycleMetaNode*>(GetPointerToHeader() + 1);

    return &metaNodes[index];
}

const TraceCycleMetaNode* TraceCycleReader::GetPointerToMeta(NodeType nodeType)
{
    const auto metaCount = GetPointerToHeader()->metaCount;

    for (int i = 0; i < metaCount; i++)
    {
        auto pMeta = GetPointerToMeta(i);

        if (pMeta->nodeType == nodeType)
        {
            return pMeta;
        }
    }

    return nullptr;
}

const void* TraceCycleReader::GetPointerToNode(NodeType nodeType)
{
    auto pMeta = GetPointerToMeta(nodeType);

    if (pMeta == nullptr)
    {
        return nullptr;
    }

    return reinterpret_cast<const uint8_t*>(m_pData) + pMeta->offset;
}
