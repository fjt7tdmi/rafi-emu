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

namespace rvtrace {

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
    CheckOffset(m_CurrentOffset);

    return reinterpret_cast<const uint8_t*>(m_pBuffer) + m_CurrentOffset;
}

int64_t MemoryTraceReader::GetCurrentCycleDataSize()
{
    auto size = GetCurrentCycleHeader()->footerOffset + sizeof(TraceCycleFooter);

    if (size < sizeof(TraceCycleHeader) + sizeof(TraceCycleFooter))
    {
        throw TraceException("detect data corruption.");
    }

    return size;
}

bool MemoryTraceReader::IsBegin()
{
    return m_CurrentOffset == 0;
}

bool MemoryTraceReader::IsEnd()
{
    return m_CurrentOffset == m_BufferSize;
}

void MemoryTraceReader::MoveToNextCycle()
{
    CheckOffset(m_CurrentOffset);

    m_CurrentOffset += GetCurrentCycleDataSize();

    if (!IsEnd())
    {
        CheckOffset(m_CurrentOffset);
    }
}

void MemoryTraceReader::MoveToPreviousCycle()
{
    if (!IsEnd())
    {
        CheckOffset(m_CurrentOffset);
    }

    auto size = GetPreviousCycleFooter()->headerOffset + sizeof(TraceCycleFooter);

    m_CurrentOffset -= size;

    CheckOffset(m_CurrentOffset);
}

void MemoryTraceReader::CheckOffset(int64_t offset)
{
    if (!(0 <= offset && offset < m_BufferSize))
    {
        std::abort();
    }
}

const TraceCycleHeader* MemoryTraceReader::GetCurrentCycleHeader()
{
    return reinterpret_cast<const TraceCycleHeader*>(GetCurrentCycleData());
}

const TraceCycleFooter* MemoryTraceReader::GetPreviousCycleFooter()
{
    auto offset = m_CurrentOffset - sizeof(TraceCycleFooter);

    CheckOffset(offset);

    auto p = reinterpret_cast<const uint8_t*>(m_pBuffer) + offset;

    return reinterpret_cast<const TraceCycleFooter*>(p);
}

}
