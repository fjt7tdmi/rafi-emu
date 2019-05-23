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

#include <rafi/trace.h>

#include "MemoryTraceReaderImpl.h"

namespace rafi { namespace trace {

MemoryTraceReaderImpl::MemoryTraceReaderImpl(const void* buffer, int64_t bufferSize)
    : m_pBuffer(buffer)
    , m_BufferSize(bufferSize)
    , m_CurrentOffset(0)
{
}

MemoryTraceReaderImpl::~MemoryTraceReaderImpl()
{
}

CycleView MemoryTraceReaderImpl::GetCycleView() const
{
    return CycleView(GetCurrentCycleData(), GetCurrentCycleDataSize());
}

bool MemoryTraceReaderImpl::IsBegin() const
{
    CheckBufferSize();

    return m_CurrentOffset == 0;
}

bool MemoryTraceReaderImpl::IsEnd() const
{
    CheckBufferSize();

    return m_CurrentOffset == m_BufferSize;
}

void MemoryTraceReaderImpl::Next()
{
    CheckBufferSize();
    CheckOffset(m_CurrentOffset);

    m_CurrentOffset += GetCurrentCycleDataSize();

    if (!IsEnd())
    {
        CheckOffset(m_CurrentOffset);
    }
}

const void* MemoryTraceReaderImpl::GetCurrentCycleData() const
{
    CheckBufferSize();
    CheckOffset(m_CurrentOffset);

    return reinterpret_cast<const uint8_t*>(m_pBuffer) + m_CurrentOffset;
}

int64_t MemoryTraceReaderImpl::GetCurrentCycleDataSize() const
{
    CheckBufferSize();

    const auto size = GetCurrentCycleHeader()->footerOffset + sizeof(CycleFooter);

    if (size < sizeof(CycleHeader) + sizeof(CycleFooter))
    {
        throw TraceException("detect data corruption. (Trace cycle size is too small)", m_CurrentOffset);
    }

    return size;
}

const CycleHeader* MemoryTraceReaderImpl::GetCurrentCycleHeader() const
{
    return reinterpret_cast<const CycleHeader*>(GetCurrentCycleData());
}

const CycleFooter* MemoryTraceReaderImpl::GetPreviousCycleFooter() const
{
    const auto offset = m_CurrentOffset - sizeof(CycleFooter);

    CheckOffset(offset);

    const auto p = reinterpret_cast<const uint8_t*>(m_pBuffer) + offset;

    return reinterpret_cast<const CycleFooter*>(p);
}

void MemoryTraceReaderImpl::CheckOffset(int64_t offset) const
{
    if (!(0 <= offset && offset <= m_BufferSize))
    {
        throw TraceException("detect data corruption. (Current offset value is out-of-range)", m_CurrentOffset);
    }
}

void MemoryTraceReaderImpl::CheckBufferSize() const
{
    if (m_BufferSize < sizeof(CycleHeader))
    {
        throw TraceException("detect data corruption. (Data size is too small)");
    }
}

}}
