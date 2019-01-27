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

#include <rvtrace/writer.h>

namespace rvtrace {

class TraceCycleBuilderImpl final
{
public:
    explicit TraceCycleBuilderImpl(int32_t flags, int csrCount, int ramSize);

    ~TraceCycleBuilderImpl();

    // Get pointer to raw data
    void* GetData();

    // Get size of raw data
    int64_t GetDataSize();

    int64_t GetNodeSize(NodeType nodeType);

    void* GetPointerToNode(NodeType nodeType);

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
    void SetNode(const FpRegNode& node);

private:
    int64_t CalculateDataSize(int32_t flags, int csrCount, int ramSize);

    int32_t CountValidFlags(int32_t flags);

    void InitializeMetaNodes(int32_t flags, int csrCount, int ramSize);

    void InitializeMetaNode(int32_t index, NodeType nodeType, int64_t offset, int csrCount, int ramSize);

    int64_t GetProperNodeSize(NodeType nodeType, int csrCount, int ramSize);

    TraceCycleHeader* GetPointerToHeader();

    TraceCycleFooter* GetPointerToFooter();

    TraceCycleMetaNode* GetPointerToMeta(int32_t index);

    TraceCycleMetaNode* GetPointerToMeta(NodeType nodeType);

    void* m_pData;

    int64_t m_DataSize;
};

}
