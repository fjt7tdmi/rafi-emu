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

#pragma once

#include "TraceCycleCommon.h"

class TraceCycleReader
{
public:
    TraceCycleReader(const void* buffer, int64_t bufferSize);

    int64_t GetOffsetOfPreviousCycle();

    int64_t GetOffsetOfNextCycle();

    const void* GetNode(NodeType nodeType);

    int64_t GetNodeSize(NodeType nodeType);

    bool IsNodeExist(NodeType nodeType);

private:
    const TraceCycleHeader* GetPointerToHeader();

    const TraceCycleMetaNode* GetPointerToMeta(int32_t index);

    const TraceCycleMetaNode* GetPointerToMeta(NodeType nodeType);

    const void* GetPointerToNode(NodeType nodeType);

    const void* m_pData;

    int64_t m_BufferSize;
};