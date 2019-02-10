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

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <numeric>

#include <rafi/common.h>

#include "TraceCycleTypes.h"

namespace rafi { namespace trace {

class TraceCycleConfig
{
public:
    TraceCycleConfig()
    {
        m_NodeCounts.fill(0);
    }

    int GetNodeCount(int nodeType) const
    {
        return m_NodeCounts.at(static_cast<int>(nodeType));
    }

    int GetNodeCount(NodeType nodeType) const
    {
        return GetNodeCount(static_cast<int>(nodeType));
    }

    int GetCsrCount() const
    {
        return m_CsrCount;
    }

    int GetRamSize() const
    {
        return m_RamSize;
    }

    int GetTotalNodeCount() const
    {
        return std::accumulate(m_NodeCounts.begin(), m_NodeCounts.end(), 0);
    }

    void SetNodeCount(int nodeType, int count)
    {
        m_NodeCounts.at(nodeType) = count;
    }

    void SetNodeCount(NodeType nodeType, int count)
    {
        SetNodeCount(static_cast<int>(nodeType), count);
    }

    void SetCsrCount(int count)
    {
        m_CsrCount = count;
    }

    void SetRamSize(int ramSize)
    {
        m_RamSize = ramSize;
    }

private:
    std::array<int, NodeTypeMax> m_NodeCounts;
    int m_CsrCount;
    int m_RamSize;
};

}}
