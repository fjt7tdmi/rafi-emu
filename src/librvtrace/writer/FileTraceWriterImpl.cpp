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

#include <cstdio>

#include <rafi/common.h>
#include <rvtrace/writer.h>

using namespace std;
using namespace rafi::common;

#include "FileTraceWriterImpl.h"

namespace rvtrace {

FileTraceWriterImpl::FileTraceWriterImpl(const char* path)
{
    m_File = std::fopen(path, "wb");
    if (m_File == nullptr)
    {
        throw FileOpenFailureException(path);
    }
}

FileTraceWriterImpl::~FileTraceWriterImpl()
{
    std::fclose(m_File);
}

void FileTraceWriterImpl::Write(void* buffer, int64_t size)
{
#if INT64_MAX > SIZE_MAX
    if (size > SIZE_MAX)
    {
        throw TraceException("argument 'size' overflow.");
    }
#endif

    std::fwrite(buffer, static_cast<size_t>(size), 1, m_File);
    std::fflush(m_File);
}

}
