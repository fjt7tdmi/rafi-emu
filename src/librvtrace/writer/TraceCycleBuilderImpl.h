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
    explicit TraceCycleBuilderImpl(const TraceCycleConfig& config);
    ~TraceCycleBuilderImpl();

    // Get pointer to raw data
    void* GetData();

    // Get size of raw data
    int64_t GetDataSize();

    int64_t GetNodeSize(NodeType nodeType);
    int64_t GetNodeSize(NodeType nodeType, int index);

    void* GetPointerToNode(NodeType nodeType);
    void* GetPointerToNode(NodeType nodeType, int index);

    void SetNode(NodeType nodeType, const void* buffer, int64_t bufferSize);
    void SetNode(NodeType nodeType, int index, const void* buffer, int64_t bufferSize);

    // utility
    void SetNode(const BasicInfoNode& node);
    void SetNode(const FpRegNode& node);
    void SetNode(const IntReg32Node& node);
    void SetNode(const IntReg64Node& node);
    void SetNode(const Pc32Node& node);
    void SetNode(const Pc64Node& node);
    void SetNode(const Trap32Node& node);
    void SetNode(const Trap64Node& node);
    void SetNode(const MemoryAccessNode& node, int index);
    void SetNode(const IoNode& node);

private:
    int64_t CalculateDataSize();

    void InitializeMetaNodes();

    int64_t GetProperNodeSize(NodeType nodeType);

    TraceCycleHeader* GetPointerToHeader();
    TraceCycleFooter* GetPointerToFooter();

    TraceCycleMetaNode* GetPointerToMeta(int32_t index);
    TraceCycleMetaNode* GetPointerToMeta(NodeType nodeType, int index);

    TraceCycleConfig m_Config;
    void* m_pData;
    int64_t m_DataSize;
};

}
