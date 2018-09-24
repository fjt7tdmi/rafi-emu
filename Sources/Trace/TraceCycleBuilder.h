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
#include "TraceCycleNode.h"

enum NodeFlag : int32_t
{
    NodeFlag_BasicInfo      = 1 << static_cast<int32_t>(NodeType::BasicInfo),
    NodeFlag_Pc32           = 1 << static_cast<int32_t>(NodeType::Pc32),
    NodeFlag_Pc64           = 1 << static_cast<int32_t>(NodeType::Pc64),
    NodeFlag_IntReg32       = 1 << static_cast<int32_t>(NodeType::IntReg32),
    NodeFlag_IntReg64       = 1 << static_cast<int32_t>(NodeType::IntReg64),
    NodeFlag_Csr32          = 1 << static_cast<int32_t>(NodeType::Csr32),
    NodeFlag_Csr64          = 1 << static_cast<int32_t>(NodeType::Csr64),
    NodeFlag_Trap32         = 1 << static_cast<int32_t>(NodeType::Trap32),
    NodeFlag_Trap64         = 1 << static_cast<int32_t>(NodeType::Trap64),
    NodeFlag_MemoryAccess32 = 1 << static_cast<int32_t>(NodeType::MemoryAccess32),
    NodeFlag_MemoryAccess64 = 1 << static_cast<int32_t>(NodeType::MemoryAccess64),
    NodeFlag_Io             = 1 << static_cast<int32_t>(NodeType::Io),
    NodeFlag_Memory         = 1 << static_cast<int32_t>(NodeType::Memory),
};

class TraceCycleBuilder final
{
public:
    explicit TraceCycleBuilder(int32_t flags);

    ~TraceCycleBuilder();

    // Get pointer to raw data
    void* GetData();

    // Get size of raw data
    int64_t GetDataSize();

    void SetOffsetOfPreviousCycle(int64_t offset);

    void SetOffsetOfNextCycle(int64_t offset);

    void SetNode(NodeType nodeType, const void* buffer, int64_t bufferSize);

    // utility
    void SetNode(const BasicInfoNode& node);
    void SetNode(const Pc32Node& node);
    void SetNode(const Pc64Node& node);
    void SetNode(const IntReg32Node& node);
    void SetNode(const IntReg64Node& node);
    void SetNode(const Trap32Node& node);
    void SetNode(const Trap64Node& node);
    void SetNode(const MemoryAccess32Node& node);
    void SetNode(const MemoryAccess64Node& node);
    void SetNode(const IoNode& node);

private:
    int64_t CalculateDataSize(int32_t flags);

    int32_t CountValidFlags(int32_t flags);

    void InitializeMetaNodes(int32_t flags);

    void InitializeMetaNode(int32_t index, NodeType nodeType, int64_t offset);

    int64_t GetProperNodeSize(NodeType nodeType);

    TraceCycleHeader* GetPointerToHeader();

    TraceCycleMetaNode* GetPointerToMeta(int32_t index);

    TraceCycleMetaNode* GetPointerToMeta(NodeType nodeType);

    void* GetPointerToNode(NodeType nodeType);

    void* m_pData;

    int64_t m_DataSize;
};