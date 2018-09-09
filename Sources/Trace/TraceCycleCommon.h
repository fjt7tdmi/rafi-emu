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

#include "../Common/BasicTypes.h"
#include "../Common/RvTypes.h"

enum class NodeType : int32_t
{
    BasicInfo = 1,
    Pc32 = 2,
    Pc64 = 3,
    IntReg32 = 4,
    IntReg64 = 5,
    Csr32 = 6,
    Csr64 = 7,
    Trap32 = 8,
    Trap64 = 9,
    MemoryAccess32 = 10,
    MemoryAccess64 = 11,
    Io = 12,
    Memory = 13,
};

struct TraceCycleHeader
{
    // Byte offset to next TraceCycleHeader from this TraceCycleHeader.
    // This value should be positive.
    // If here is the last cycle, this value is 0.
    int64_t next;

    // Byte offset to preivious TraceCycleHeader from this TraceCycleHeader.
    // This value should be negative.
    // If here is the first cycle, this value is 0.
    int64_t prev;

    // Number of TraceCycleMetaNode
    int32_t metaCount;

    int32_t reserved;
};

struct TraceCycleMetaNode
{
    NodeType nodeType;

    int32_t reserved;

    // Byte size of the node
    int64_t size;
};
