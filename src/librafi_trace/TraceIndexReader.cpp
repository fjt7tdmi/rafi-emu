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

#include <rafi/trace.h>

#include "TraceIndexReaderImpl.h"

namespace rafi { namespace trace {

TraceIndexReader::TraceIndexReader(const char* path)
{
    m_pImpl = new TraceIndexReaderImpl(path);
}

TraceIndexReader::~TraceIndexReader()
{
    delete m_pImpl;
}

const ICycle* TraceIndexReader::GetCycle() const
{
    return m_pImpl->GetCycle();
}

bool TraceIndexReader::IsEnd() const
{
    return m_pImpl->IsEnd();
}

void TraceIndexReader::Next()
{
    m_pImpl->Next();
}

void TraceIndexReader::Next(uint32_t cycle)
{
    m_pImpl->Next(cycle);
}

}}
