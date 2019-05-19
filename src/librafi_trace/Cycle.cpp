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

#include <cassert>
#include <cstddef>
#include <cstdlib>

#include <rafi/trace.h>

namespace rafi { namespace trace {

Cycle::Cycle()
{
}

Cycle::Cycle(const void* buffer, size_t bufferSize)
{
    m_pBuffer = std::malloc(bufferSize);
    m_BufferSize = bufferSize;

    if (!m_pBuffer)
    {
        std::fprintf(stderr, "Out of memory.\n");
        std::exit(1);
    }

    std::memcpy(m_pBuffer, buffer, bufferSize);

    m_pView = new CycleReader(m_pBuffer, m_BufferSize);
}

Cycle::~Cycle()
{
    if (m_pView)
    {
        delete m_pView;
    }
    if (m_pBuffer)
    {
        std::free(m_pBuffer);
    }
}

const IoNode* Cycle::GetIoNode() const
{
    return m_pView->GetIoNode();
}

}}
