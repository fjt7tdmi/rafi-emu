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

#include <algorithm>
#include <climits>
#include <cstring>
#include <fstream>

#if defined(__GNUC__)
#include <experimental/filesystem>
#else
#include <filesystem>
#endif

#include "../Common/Exception.h"

#include "TraceBinary.h"
#include "TraceBinaryReader.h"

namespace fs = std::experimental::filesystem;

TraceBinaryReader::TraceBinaryReader(const char* path)
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

    const auto size = static_cast<size_t>(fileSize);

    m_Buffer = new char[size];

    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    ifs.read(m_Buffer, size);

    const auto pHeader = reinterpret_cast<TraceBinaryHeader*>(m_Buffer);
    if (std::memcmp(&pHeader->signature, TraceBinarySignature, sizeof(TraceBinarySignature)) != 0)
    {
        throw TraceException("Signature vefication failed. Is this a really RV trace binary file?");
    }

    m_Cursor = m_Buffer + pHeader->headerSize;
}

TraceBinaryReader::~TraceBinaryReader()
{
    delete[] m_Buffer;
}

char* TraceBinaryReader::GetNode()
{
    return m_Cursor;
}

size_t TraceBinaryReader::GetNodeSize() const
{
    const auto pHeader = reinterpret_cast<TraceHeader*>(m_Cursor);
    return static_cast<size_t>(pHeader->nodeSize);
}

bool TraceBinaryReader::IsEndNode() const
{
    const auto pHeader = reinterpret_cast<TraceHeader*>(m_Cursor);
    return pHeader->nodeSize == 0;
}

void TraceBinaryReader::MoveToNext()
{
    if (IsEndNode())
    {
        return;
    }

    const auto pHeader = reinterpret_cast<TraceHeader*>(m_Cursor);

    if (pHeader->nodeSize < 0)
    {
        throw TraceException("Detect minus node size. Maybe input file is corruptted.");
    }
#if INT64_MAX > LONG_MAX
    if (pHeader->nodeSize > LONG_MAX)
    {
        throw TraceException("Detect too big node size.");
    }
#endif

    m_Cursor += (pHeader->nodeSize);
}

void TraceBinaryReader::MoveToFirst()
{
    const auto pHeader = reinterpret_cast<TraceBinaryHeader*>(m_Buffer);
    m_Cursor = m_Buffer + pHeader->headerSize;
}

void TraceBinaryReader::MoveToLast()
{
    if (IsEndNode())
    {
        MoveToFirst();
    }

    for (;;)
    {
        const auto pHeader = reinterpret_cast<TraceHeader*>(m_Cursor);

        if (pHeader->nodeSize < 0)
        {
            throw TraceException("Detect minus node size. Maybe input file is corruptted.");
        }

#if INT64_MAX > LONG_MAX
        if (pHeader->nodeSize > LONG_MAX)
        {
            throw TraceException("Detect too big node size.");
        }
#endif

        const auto nodeSize = pHeader->nodeSize;
        m_Cursor += nodeSize;

        if (IsEndNode())
        {
            m_Cursor -= nodeSize;
            return;
        }
    }
}
