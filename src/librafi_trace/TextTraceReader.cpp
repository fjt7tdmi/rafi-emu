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

#include "TextTraceReaderImpl.h"

namespace rafi { namespace trace {

TextTraceReader::TextTraceReader(const char* path)
{
    m_pImpl = new TextTraceReaderImpl(path);
}

TextTraceReader::~TextTraceReader()
{
    if (m_pImpl != nullptr)
    {
        delete m_pImpl;
        m_pImpl = nullptr;
    }
}

CycleView TextTraceReader::GetCycleView() const
{
    return m_pImpl->GetCycleView();
}

bool TextTraceReader::IsBegin() const
{
    return m_pImpl->IsBegin();
}

bool TextTraceReader::IsEnd() const
{
    return m_pImpl->IsEnd();
}

void TextTraceReader::Next()
{
    m_pImpl->Next();
}

}}
