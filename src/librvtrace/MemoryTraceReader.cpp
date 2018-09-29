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

#include <cstdint>
#include <cstdlib>

#include <rvtrace/reader.h>

MemoryTraceReader::MemoryTraceReader(const void* buffer, int64_t bufferSize)
    : m_pBuffer(buffer)
    , m_BufferSize(bufferSize)
    , m_CurrentOffset(0)
{
}

MemoryTraceReader::~MemoryTraceReader()
{
}

const void* MemoryTraceReader::GetCurrentCycleData()
{
    if (m_CurrentOffset + sizeof(TraceCycleHeader) > m_BufferSize)
    {
        throw TraceException("detect data corruption.");
    }

    return reinterpret_cast<const uint8_t*>(m_pBuffer) + m_CurrentOffset;
}

int64_t MemoryTraceReader::GetCurrentCycleDataSize()
{
    auto header = reinterpret_cast<const TraceCycleHeader*>(GetCurrentCycleData());

    return header->size;
}

bool MemoryTraceReader::IsFirstCycle()
{
    auto header = reinterpret_cast<const TraceCycleHeader*>(GetCurrentCycleData());

    return header->prev == 0;
}

bool MemoryTraceReader::IsLastCycle()
{
    auto header = reinterpret_cast<const TraceCycleHeader*>(GetCurrentCycleData());

    return header->next == 0;
}

void MemoryTraceReader::MoveNextCycle()
{
    auto header = reinterpret_cast<const TraceCycleHeader*>(GetCurrentCycleData());

    if (header->next < 0)
    {
        throw TraceException("detect data corruption.");
    }
    else if (header->next == 0)
    {
        throw TraceException("cannot move to next cycle from the last cycle.");
    }

    m_CurrentOffset += header->next;

    CheckCurrentOffset();
}

void MemoryTraceReader::MovePreviousCycle()
{
    auto header = reinterpret_cast<const TraceCycleHeader*>(GetCurrentCycleData());

    if (header->prev > 0)
    {
        throw TraceException("detect data corruption.");
    }
    else if (header->prev == 0)
    {
        throw TraceException("cannot move to preivious cycle from the first cycle.");
    }

    m_CurrentOffset += header->prev;

    CheckCurrentOffset();
}

void MemoryTraceReader::CheckCurrentOffset()
{
    if (!(0 <= m_CurrentOffset && m_CurrentOffset < m_BufferSize))
    {
        throw TraceException("detect data corruption.");
    }
}
