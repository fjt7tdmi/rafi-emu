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
#include <cstring>
#include <sstream>

#include <rafi/trace.h>

#include "TraceIndexWriterImpl.h"

namespace rafi { namespace trace {

TraceIndexWriterImpl::TraceIndexWriterImpl(const char* pathBase)
    : m_PathBase(pathBase)
{
    const auto path = std::string(pathBase) + ".tidx";

    m_pData = (char*)std::malloc(MaxFileSize);

    m_pIndexFile = std::fopen(path.c_str(), "w");
    if (m_pIndexFile == nullptr)
    {
        throw FileOpenFailureException(path.c_str());
    }
}

TraceIndexWriterImpl::~TraceIndexWriterImpl()
{
    FlushData();

    std::fclose(m_pIndexFile);

    std::free(m_pData);
}

void TraceIndexWriterImpl::Write(void* buffer, int64_t bufferSize)
{
#if INT64_MAX > SIZE_MAX
    if (size > SIZE_MAX)
    {
        throw TraceException("argument 'size' overflow.");
    }
#endif

    const auto size = static_cast<size_t>(bufferSize);

    if (size > MaxFileSize)
    {
        throw TraceException("argument 'size' is larger than MaxFileSize.");
    }

    if (m_DataSize + size > MaxFileSize)
    {
        FlushData();
    }

    std::memcpy(&m_pData[m_DataSize], buffer, size);
    m_DataSize += size;
    m_CycleCount++;
}

void TraceIndexWriterImpl::FlushData()
{
    if (m_DataSize == 0)
    {
        return;
    }

    // Generate data file path
    std::stringstream ss;
    ss << m_PathBase << "." << m_DataFileCount << ".tbin";

    const auto path = ss.str();

    // Write data
    auto fp = std::fopen(path.c_str(), "wb");
    if (fp == nullptr)
    {
        throw FileOpenFailureException(path.c_str());
    }

    std::fwrite(m_pData, m_DataSize, 1, fp);
    std::fclose(fp);

    // Write path to index file
    std::fprintf(m_pIndexFile, "%s %d\n", path.c_str(), m_CycleCount);

    m_DataSize = 0;
    m_CycleCount = 0;
    m_DataFileCount++;
}

}}
