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

#if defined(__GNUC__)
#include <experimental/filesystem>
#else
#include <filesystem>
#endif

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>

#include <rafi/trace.h>

#include "TextTraceReaderImpl.h"

namespace fs = std::experimental::filesystem;

namespace rafi { namespace trace {

TextTraceReaderImpl::TextTraceReaderImpl(const char* path)
{
    m_pInput = new std::ifstream(path, std::ios::in);
}

TextTraceReaderImpl::~TextTraceReaderImpl()
{
    delete m_pInput;
}

CycleView TextTraceReaderImpl::GetCycleView() const
{
    return CycleView(m_pCycleBuilder->GetData(), m_pCycleBuilder->GetDataSize());
}

bool TextTraceReaderImpl::IsBegin() const
{
    return m_IsBegin;
}

bool TextTraceReaderImpl::IsEnd() const
{
    return m_IsEnd;
}

void TextTraceReaderImpl::Next()
{
    if (m_IsEnd)
    {
        throw TraceException("TextTraceReaderImpl reached the end of input.");
    }

    m_pTextCycle = TextCycle::Parse(*m_pInput);

    m_pCycleBuilder = std::make_unique<CycleBuilder>(m_pTextCycle->GetCycleConfig());


}

}}
