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

MemoryTraceReader::MemoryTraceReader(const void* buffer, int64_t bufferSize)
{
    m_pImpl = new MemoryTraceReaderImpl(buffer, bufferSize);
}

MemoryTraceReader::~MemoryTraceReader()
{
    delete m_pImpl;
}

const void* MemoryTraceReader::GetCurrentCycleData()
{
    return m_pImpl->GetCurrentCycleData();
}

int64_t MemoryTraceReader::GetCurrentCycleDataSize()
{
    return m_pImpl->GetCurrentCycleDataSize();
}

bool MemoryTraceReader::IsBegin()
{
    return m_pImpl->IsBegin();
}

bool MemoryTraceReader::IsEnd()
{
    return m_pImpl->IsEnd();
}

void MemoryTraceReader::Next()
{
    m_pImpl->Next();
}

}}
