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

#pragma once

#include <cstdint>
#include <memory>

#include <rafi/trace.h>

#include "BinaryCycle.h"

namespace rafi { namespace trace {

class TraceBinaryMemoryReaderImpl
{
public:
    TraceBinaryMemoryReaderImpl(const void* buffer, size_t bufferSize);
    ~TraceBinaryMemoryReaderImpl();

    const ICycle* GetCycle() const;

    bool IsEnd() const;

    void Next();

private:
    void CheckBufferSize() const;
    void CheckOffset() const;

    const void* m_pBuffer{ nullptr };
    size_t m_BufferSize{ 0 };
    size_t m_Offset{ 0 };
    std::unique_ptr<BinaryCycle> m_pCycle{ nullptr };
};

}}
