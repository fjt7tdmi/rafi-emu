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

#include <rafi/common.h>

#include "CycleTypes.h"

namespace rafi { namespace trace {

class BinaryCycleLoggerImpl;

class BinaryCycleLogger final
{
public:
    BinaryCycleLogger(uint32_t cycle, const XLEN& xlen, uint64_t pc);
    ~BinaryCycleLogger();

    void Add(const NodeIntReg32& value);
    void Add(const NodeIntReg64& value);
    void Add(const NodeFpReg& value);
    void Add(const NodeIo& value);
    void Add(const NodeOpEvent& value);
    void Add(const NodeTrapEvent& value);
    void Add(const NodeMemoryEvent& value);

    void Break();

    // Get pointer to raw data
    void* GetData();

    // Get size of raw data
    size_t GetDataSize() const;

private:
    BinaryCycleLoggerImpl* m_pImpl;
};

}}
