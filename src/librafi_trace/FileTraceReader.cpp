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

#include "FileTraceReaderImpl.h"

namespace rafi { namespace trace {

FileTraceReader::FileTraceReader(const char* path)
{
    m_pImpl = new FileTraceReaderImpl(path);
}

FileTraceReader::~FileTraceReader()
{
    delete m_pImpl;
}

const ICycle* FileTraceReader::GetCycle() const
{
    return nullptr;
}

bool FileTraceReader::IsEnd() const
{
    return m_pImpl->IsEnd();
}

void FileTraceReader::Next()
{
    m_pImpl->Next();
}

CycleView FileTraceReader::GetCycleView() const
{
    return m_pImpl->GetCycleView();
}

}}
