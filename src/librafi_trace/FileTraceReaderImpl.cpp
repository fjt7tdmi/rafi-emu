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
#include <fstream>

#include <rafi/trace.h>

#include "FileTraceReaderImpl.h"

namespace fs = std::experimental::filesystem;

namespace rafi { namespace trace {

FileTraceReaderImpl::FileTraceReaderImpl(const char* path)
{
    uintmax_t fileSize;

    try
    {
        fileSize = static_cast<int>(fs::file_size(path));
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

#if UINTMAX_MAX > INT64_MAX
    if (fileSize > INT64_MAX)
    {
        throw FileOpenFailureException(path);
    }
#endif

    const auto size = static_cast<size_t>(fileSize);

    m_pBuffer = new char[size];
    m_BufferSize = static_cast<int64_t>(fileSize);

    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    ifs.read(m_pBuffer, size);

    m_pMemoryTraceReader = new MemoryTraceReaderImpl(m_pBuffer, m_BufferSize);
}

FileTraceReaderImpl::~FileTraceReaderImpl()
{
    delete m_pMemoryTraceReader;
    delete[] m_pBuffer;
}

const void* FileTraceReaderImpl::GetCurrentCycleData()
{
    return m_pMemoryTraceReader->GetCurrentCycleData();
}

int64_t FileTraceReaderImpl::GetCurrentCycleDataSize()
{
    return m_pMemoryTraceReader->GetCurrentCycleDataSize();
}

bool FileTraceReaderImpl::IsBegin()
{
    return m_pMemoryTraceReader->IsBegin();
}

bool FileTraceReaderImpl::IsEnd()
{
    return m_pMemoryTraceReader->IsEnd();
}

void FileTraceReaderImpl::MoveToNextCycle()
{
    m_pMemoryTraceReader->MoveToNextCycle();
}

void FileTraceReaderImpl::MoveToPreviousCycle()
{
    m_pMemoryTraceReader->MoveToPreviousCycle();
}

}}