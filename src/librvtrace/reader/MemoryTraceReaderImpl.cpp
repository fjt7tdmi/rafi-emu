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

#include "MemoryTraceReaderImpl.h"

namespace rvtrace {

MemoryTraceReaderImpl::MemoryTraceReaderImpl(const void* buffer, int64_t bufferSize)
    : m_pBuffer(buffer)
    , m_BufferSize(bufferSize)
    , m_CurrentOffset(0)
{
}

MemoryTraceReaderImpl::~MemoryTraceReaderImpl()
{
}

const void* MemoryTraceReaderImpl::GetCurrentCycleData()
{
    CheckOffset(m_CurrentOffset);

    return reinterpret_cast<const uint8_t*>(m_pBuffer) + m_CurrentOffset;
}

int64_t MemoryTraceReaderImpl::GetCurrentCycleDataSize()
{
    const auto size = GetCurrentCycleHeader()->footerOffset + sizeof(TraceCycleFooter);

    if (size < sizeof(TraceCycleHeader) + sizeof(TraceCycleFooter))
    {
        throw TraceException("detect data corruption.");
    }

    return size;
}

bool MemoryTraceReaderImpl::IsBegin()
{
    return m_CurrentOffset == 0;
}

bool MemoryTraceReaderImpl::IsEnd()
{
    return m_CurrentOffset == m_BufferSize;
}

void MemoryTraceReaderImpl::MoveToNextCycle()
{
    CheckOffset(m_CurrentOffset);

    m_CurrentOffset += GetCurrentCycleDataSize();

    if (!IsEnd())
    {
        CheckOffset(m_CurrentOffset);
    }
}

void MemoryTraceReaderImpl::MoveToPreviousCycle()
{
    if (!IsEnd())
    {
        CheckOffset(m_CurrentOffset);
    }

    const auto size = GetPreviousCycleFooter()->headerOffset + sizeof(TraceCycleFooter);

    m_CurrentOffset -= size;

    CheckOffset(m_CurrentOffset);
}

void MemoryTraceReaderImpl::CheckOffset(int64_t offset)
{
    if (!(0 <= offset && offset < m_BufferSize))
    {
        std::abort();
    }
}

const TraceCycleHeader* MemoryTraceReaderImpl::GetCurrentCycleHeader()
{
    return reinterpret_cast<const TraceCycleHeader*>(GetCurrentCycleData());
}

const TraceCycleFooter* MemoryTraceReaderImpl::GetPreviousCycleFooter()
{
    const auto offset = m_CurrentOffset - sizeof(TraceCycleFooter);

    CheckOffset(offset);

    const auto p = reinterpret_cast<const uint8_t*>(m_pBuffer) + offset;

    return reinterpret_cast<const TraceCycleFooter*>(p);
}

}