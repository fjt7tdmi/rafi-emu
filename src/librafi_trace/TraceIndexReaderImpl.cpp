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
    }
}

const ICycle* TraceIndexReaderImpl::GetCycle() const
{
    return m_pTraceBinary->GetCycle();
}

bool TraceIndexReaderImpl::IsEnd() const
{
    return m_Index == m_Entries.size();
}

void TraceIndexReaderImpl::Next()
{
    m_pTraceBinary->Next();

    if (m_pTraceBinary->IsEnd())
    {
        m_Index++;
        UpdateTraceBinary();
    }
}

void TraceIndexReaderImpl::ParseIndexFile(const char* path)
{
    auto f = std::ifstream(path);

    while (!f.eof())
    {
        Entry entry;
        f >> entry.path;

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
    const auto path = m_Entries[m_Index].path;

    m_pTraceBinary = new TraceBinaryReaderImpl(path.c_str());
}

}}
