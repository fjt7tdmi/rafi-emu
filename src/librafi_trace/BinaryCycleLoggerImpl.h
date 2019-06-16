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

#include <rafi/trace.h>

namespace rafi { namespace trace {

class BinaryCycleLoggerImpl final
{
public:
    BinaryCycleLoggerImpl(uint32_t cycle, const XLEN& xlen, uint64_t pc);
    ~BinaryCycleLoggerImpl();

    void Add(const NodeIntReg32& node);
    void Add(const NodeIntReg64& node);
    void Add(const NodeFpReg& node);
    void Add(const NodeIo& node);
    void Add(const NodeOpEvent& node);
    void Add(const NodeTrapEvent& node);
    void Add(const NodeMemoryEvent& node);

    void Break();

    // Get pointer to raw data
    void* GetData();

    // Get size of raw data
    size_t GetDataSize() const;

private:
    void Add(const NodeBasic& node);
    void AddData(uint16_t nodeId, const void* pNode, size_t nodeSize);

    void* m_pBuffer{ nullptr };
    size_t m_BufferSize{ 0 };
    size_t m_DataSize{ 0 };
};


}}
