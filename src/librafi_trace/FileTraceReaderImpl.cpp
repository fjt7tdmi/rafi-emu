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

#include <rafi/trace.h>

#include "FileTraceReaderImpl.h"

namespace fs = std::experimental::filesystem;

namespace rafi { namespace trace {

FileTraceReaderImpl::FileTraceReaderImpl(const char* path)
{
    uintmax_t fileSize;

    try
    {
        fileSize = fs::file_size(path);
    }
    catch (fs::filesystem_error&)
    {
        throw FileOpenFailureException(path);
    }

#if UINTMAX_MAX > SIZE_MAX
    if (fileSize > SIZE_MAX)
    {
        throw FileOpenFailureException(path);
    }
#endif

    m_pBuffer = malloc(fileSize);
    if (m_pBuffer == nullptr)
    {
        throw TraceException("Failed to allocate memory.\n");
    }
    m_BufferSize = fileSize;

    auto fp = std::fopen(path, "rb");

    auto n = std::fread(m_pBuffer, m_BufferSize, 1, fp);
    if (n != m_BufferSize)
    {
        throw TraceException("Failed to read file.\n");
    }

    std::fclose(fp);

    m_pImpl = new MemoryTraceReader(m_pBuffer, m_BufferSize);
}

FileTraceReaderImpl::~FileTraceReaderImpl()
{
    if (m_pImpl != nullptr)
    {
        delete m_pImpl;
    }
    if (m_pBuffer != nullptr)
    {
        free(m_pBuffer);
    }
}

const ICycle* FileTraceReaderImpl::GetCycle() const
{
    return m_pImpl->GetCycle();
}

bool FileTraceReaderImpl::IsEnd() const
{
    return m_pImpl->IsEnd();
}

void FileTraceReaderImpl::Next()
{
    m_pImpl->Next();
}

}}
