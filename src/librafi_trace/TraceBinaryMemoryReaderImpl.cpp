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

#include "TraceBinaryMemoryReaderImpl.h"

namespace rafi { namespace trace {

TraceBinaryMemoryReaderImpl::TraceBinaryMemoryReaderImpl(const void* buffer, size_t bufferSize)
    : m_pBuffer(buffer)
    , m_BufferSize(bufferSize)
{
    CheckBufferSize();

    m_pCycle = BinaryCycle::Parse(m_pBuffer, m_BufferSize);
}

TraceBinaryMemoryReaderImpl::~TraceBinaryMemoryReaderImpl()
{
    m_pCycle = nullptr;
}

const ICycle* TraceBinaryMemoryReaderImpl::GetCycle() const
{
    return m_pCycle.get();
}

bool TraceBinaryMemoryReaderImpl::IsEnd() const
{
    return m_Offset == m_BufferSize;
}

void TraceBinaryMemoryReaderImpl::Next()
{
    m_Offset += m_pCycle->GetSize();

    CheckOffset();

    if (!IsEnd())
    {
        m_pCycle = BinaryCycle::Parse(reinterpret_cast<const uint8_t*>(m_pBuffer) + m_Offset, m_BufferSize - m_Offset);
    }
    else
    {
        m_pCycle = nullptr;
    }
}

void TraceBinaryMemoryReaderImpl::CheckOffset() const
{
    if (!(0 <= m_Offset && m_Offset <= m_BufferSize))
    {
        throw TraceException("detect data corruption. (Current offset value is out-of-range)", m_Offset);
    }
}

void TraceBinaryMemoryReaderImpl::CheckBufferSize() const
{
    if (m_BufferSize < sizeof(NodeHeader))
    {
        throw TraceException("detect data corruption. (Data size is too small)");
    }
}

}}
