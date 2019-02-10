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

#include <stdint.h>

#include <rafi/common.h>

namespace rafi { namespace trace {

class TraceCycleReaderImpl;

class TraceCycleReader
{
public:
    TraceCycleReader(const void* buffer, int64_t bufferSize);
    ~TraceCycleReader();

    int64_t GetOffsetOfPreviousCycle() const;
    int64_t GetOffsetOfNextCycle() const;

    const void* GetNode(NodeType nodeType) const;
    const void* GetNode(NodeType nodeType, int index) const;

    int64_t GetNodeSize(NodeType nodeType) const;
    int64_t GetNodeSize(NodeType nodeType, int index) const;

    int GetNodeCount(NodeType nodeType) const;

    // utility
    const BasicInfoNode* GetBasicInfoNode() const;
    const FpRegNode* GetFpRegNode() const;
    const IntReg32Node* GetIntReg32Node() const;
    const IntReg64Node* GetIntReg64Node() const;
    const Pc32Node* GetPc32Node() const;
    const Pc64Node* GetPc64Node() const;
    const Csr32Node* GetCsr32Node() const;
    const Csr64Node* GetCsr64Node() const;
    const Trap32Node* GetTrap32Node() const;
    const Trap64Node* GetTrap64Node() const;
    const MemoryAccessNode* GetMemoryAccessNode(int index) const;
    const IoNode* GetIoNode() const;
    const void* GetMemoryNode() const;

private:
    TraceCycleReaderImpl* m_pImpl;
};

}}
