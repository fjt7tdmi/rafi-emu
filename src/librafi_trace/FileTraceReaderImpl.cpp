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

#if UINTMAX_MAX > INT64_MAX
    if (fileSize > INT64_MAX)
    {
        throw FileOpenFailureException(path);
    }
#endif

    m_pStream = new std::ifstream(path, std::ios::in | std::ios::binary);

    m_FileSize = static_cast<int64_t>(fileSize);
    m_Offset = 0;

    UpdateCycleData();
}

FileTraceReaderImpl::~FileTraceReaderImpl()
{
    if (m_pCycleData != nullptr)
    {
        delete m_pCycleData;
        m_pCycleData = nullptr;
    }

    delete m_pStream;
    m_pStream = nullptr;
}

CycleView FileTraceReaderImpl::GetCycleView() const
{
    return CycleView(m_pCycleData, m_CycleDataSize);
}

bool FileTraceReaderImpl::IsEnd() const
{
    return m_Offset == m_FileSize;
}

void FileTraceReaderImpl::Next()
{
    CheckOffset(m_Offset);

    m_Offset += m_CycleDataSize;

    if (!IsEnd())
    {
        CheckOffset(m_Offset);
    }

    UpdateCycleData();
}

void FileTraceReaderImpl::UpdateCycleData()
{
    if (m_pCycleData != nullptr)
    {
        m_CycleDataSize = 0;

        delete m_pCycleData;
        m_pCycleData = nullptr;
    }

    if (IsEnd())
    {
        return;
    }

    const auto header = GetCurrentCycleHeader();

    m_CycleDataSize = header.footerOffset + sizeof(CycleFooter);
    m_pCycleData = new char[static_cast<size_t>(m_CycleDataSize)];

    CheckOffset(m_Offset);
    CheckOffset(m_Offset + m_CycleDataSize);

    m_pStream->seekg(m_Offset, std::ios_base::beg);
    m_pStream->read(m_pCycleData, m_CycleDataSize);
}

void FileTraceReaderImpl::CheckOffset(int64_t offset) const
{
    if (!(0 <= offset && offset <= m_FileSize))
    {
        throw TraceException("detect data corruption. (Current offset value is out-of-range)", m_Offset);
    }
}

CycleHeader FileTraceReaderImpl::GetCurrentCycleHeader()
{
    CycleHeader header;

    CheckOffset(m_Offset);
    CheckOffset(m_Offset + sizeof(header));

    m_pStream->seekg(m_Offset, std::ios_base::beg);
    m_pStream->read(reinterpret_cast<char*>(&header), sizeof(header));

    return header;
}

CycleFooter FileTraceReaderImpl::GetPreviousCycleFooter()
{
    CycleFooter footer;

    CheckOffset(m_Offset - sizeof(footer));
    CheckOffset(m_Offset);

    m_pStream->seekg(m_Offset - sizeof(footer), std::ios_base::beg);
    m_pStream->read(reinterpret_cast<char*>(&footer), sizeof(footer));

    return footer;
}

}}
