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

#include <rafi/trace.h>

namespace rafi { namespace trace {

class MemoryTraceReaderImpl
{
public:
    MemoryTraceReaderImpl(const void* buffer, int64_t bufferSize);
    ~MemoryTraceReaderImpl();

    const void* GetCurrentCycleData();
    int64_t GetCurrentCycleDataSize();

    bool IsBegin();
    bool IsEnd();

    void MoveToNextCycle();
    void MoveToPreviousCycle();

private:
    const CycleHeader* GetCurrentCycleHeader();
    const CycleFooter* GetPreviousCycleFooter();

    void CheckBufferSize();
    void CheckOffset(int64_t offset);

    const void* m_pBuffer;
    int64_t m_BufferSize;

    int64_t m_CurrentOffset;
};

}}