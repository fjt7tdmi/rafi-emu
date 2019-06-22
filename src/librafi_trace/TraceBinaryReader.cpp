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

#include "TraceBinaryReaderImpl.h"

namespace rafi { namespace trace {

TraceBinaryReader::TraceBinaryReader(const char* path)
{
    m_pImpl = new TraceBinaryReaderImpl(path);
}

TraceBinaryReader::~TraceBinaryReader()
{
    delete m_pImpl;
}

const ICycle* TraceBinaryReader::GetCycle() const
{
    return m_pImpl->GetCycle();
}

bool TraceBinaryReader::IsEnd() const
{
    return m_pImpl->IsEnd();
}

void TraceBinaryReader::Next()
{
    m_pImpl->Next();
}

void TraceBinaryReader::Next(uint32_t cycle)
{
    for (uint32_t i = 0; i < cycle; i++)
    {
        Next();
    }
}

}}
