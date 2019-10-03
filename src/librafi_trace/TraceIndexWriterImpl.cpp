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
#include <sstream>

#include <rafi/trace.h>

#include "TraceIndexWriterImpl.h"

namespace rafi { namespace trace {

TraceIndexWriterImpl::TraceIndexWriterImpl(const char* pathBase)
    : m_PathBase(pathBase)
{
    const auto path = std::string(pathBase) + ".tidx";

    m_pBuffer = (char*)std::malloc(BufferSize);

    m_pIndexFile = std::fopen(path.c_str(), "w");
    if (m_pIndexFile == nullptr)
    {
        throw FileOpenFailureException(path.c_str());
    }
    std::setvbuf(m_pIndexFile, m_pBuffer, _IOFBF, BufferSize);

    OpenBinaryFile();
}

TraceIndexWriterImpl::~TraceIndexWriterImpl()
{
    CloseBinaryFile();

    std::fflush(m_pBinaryFile);
    std::fclose(m_pIndexFile);

    std::free(m_pBuffer);
}

void TraceIndexWriterImpl::Write(void* buffer, int64_t size)
{
#if INT64_MAX > SIZE_MAX
    if (size > SIZE_MAX)
    {
        throw TraceException("argument 'size' overflow.");
    }
#endif

    std::fwrite(buffer, static_cast<size_t>(size), 1, m_pBinaryFile);

    m_CycleCount++;

    if (m_CycleCount == MaxCycleCount)
    {
        CloseBinaryFile();
        OpenBinaryFile();
    }
}

void TraceIndexWriterImpl::OpenBinaryFile()
{
    std::stringstream ss;
    ss << m_PathBase << "." << m_BinaryFileCount << ".tbin";

    const auto path = ss.str();

    m_pBinaryFile = std::fopen(path.c_str(), "wb");
    if (m_pBinaryFile == nullptr)
    {
        throw FileOpenFailureException(path.c_str());
    }

    // write path to index file
    std::fprintf(m_pIndexFile, "%s\n", path.c_str());
}

void TraceIndexWriterImpl::CloseBinaryFile()
{
    if (m_pBinaryFile)
    {
        std::fclose(m_pBinaryFile);
        m_BinaryFileCount++;

        // write cycle count to index file
        std::fprintf(m_pIndexFile, "%d\n", m_CycleCount);
        m_CycleCount = 0;
    }
}

}}
