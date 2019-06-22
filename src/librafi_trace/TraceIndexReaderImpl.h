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

#include <vector>

#include <rafi/trace.h>

#include "BinaryCycle.h"
#include "TraceBinaryReaderImpl.h"

namespace rafi { namespace trace {

class TraceIndexReaderImpl final
{
public:
    TraceIndexReaderImpl(const char* path);
    ~TraceIndexReaderImpl();

    const ICycle* GetCycle() const;

    bool IsEnd() const;

    void Next();

private:
    void ParseIndexFile(const char* path);
    void UpdateTraceBinary();

    struct Entry
    {
        std::string path;
        int cycle;
    };

    std::vector<Entry> m_Entries;

    int m_Index{ 0 }; // current index of m_Entries

    TraceBinaryReaderImpl* m_pTraceBinary{ nullptr };
};

}}
