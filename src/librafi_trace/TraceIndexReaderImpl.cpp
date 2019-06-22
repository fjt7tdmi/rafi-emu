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

#include <fstream>

#include <rafi/trace.h>

#include "TraceIndexReaderImpl.h"

namespace rafi { namespace trace {

TraceIndexReaderImpl::TraceIndexReaderImpl(const char* path)
{
    ParseIndexFile(path);
    UpdateTraceBinary();
}

TraceIndexReaderImpl::~TraceIndexReaderImpl()
{
    if (m_pTraceBinary != nullptr)
    {
        delete m_pTraceBinary;
        m_pTraceBinary = nullptr;
    }
}

const ICycle* TraceIndexReaderImpl::GetCycle() const
{
    return m_pTraceBinary->GetCycle();
}

bool TraceIndexReaderImpl::IsEnd() const
{
    return m_EntryIndex == m_Entries.size();
}

void TraceIndexReaderImpl::Next()
{
    m_pTraceBinary->Next();
    m_Cycle++;

    if (!m_pTraceBinary->IsEnd())
    {
        return;
    }

    m_EntryIndex++;
    if (m_EntryIndex == m_Entries.size())
    {
        return;
    }

    UpdateTraceBinary();
}

void TraceIndexReaderImpl::Next(uint32_t cycle)
{
    m_Cycle += cycle;

    int entryIndex = -1;

    uint32_t skippedCycle = 0;
    for (int i = 0; i < m_Entries.size(); i++)
    {
        if (skippedCycle + m_Entries[i].cycle > m_Cycle)
        {
            entryIndex = i;
            return;
        }

        skippedCycle += m_Entries[i].cycle;
    }

    if (entryIndex < 0)
    {
        throw TraceException("Failed to skip specified cycles in TraceIndexReaderImpl::Next()");
    }

    m_EntryIndex = entryIndex;
    UpdateTraceBinary();

    for (uint32_t i = skippedCycle; i < m_Cycle; i++)
    {
        Next();
    }
}

void TraceIndexReaderImpl::ParseIndexFile(const char* path)
{
    auto f = std::ifstream(path);

    while (!f.eof())
    {
        Entry entry;
        
        f >> entry.path;
        if (entry.path.empty())
        {
            continue;
        }

        if (!f.eof())
        {
            f >> entry.cycle;
        }
        else
        {
            entry.cycle = 0;
        }

        m_Entries.push_back(entry);
    }
}

void TraceIndexReaderImpl::UpdateTraceBinary()
{
    if (m_pTraceBinary != nullptr)
    {
        delete m_pTraceBinary;
        m_pTraceBinary = nullptr;
    }

    const auto path = m_Entries[m_EntryIndex].path;

    m_pTraceBinary = new TraceBinaryReaderImpl(path.c_str());
}

}}
