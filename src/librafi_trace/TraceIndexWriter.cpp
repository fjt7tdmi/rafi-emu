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

#include "TraceIndexWriterImpl.h"

namespace rafi { namespace trace {

TraceIndexWriter::TraceIndexWriter(const char* pathBase)
{
    m_pImpl = new TraceIndexWriterImpl(pathBase);
}

TraceIndexWriter::~TraceIndexWriter()
{
    delete m_pImpl;
}

void TraceIndexWriter::Write(void* buffer, int64_t size)
{
    m_pImpl->Write(buffer, size);
}

}}
