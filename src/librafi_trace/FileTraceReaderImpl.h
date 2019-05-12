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

#include "MemoryTraceReaderImpl.h"

namespace rafi { namespace trace {

class FileTraceReaderImpl final
{
public:
    FileTraceReaderImpl(const char* path);
    ~FileTraceReaderImpl();

    const void* GetCurrentCycleData();
    int64_t GetCurrentCycleDataSize();

    bool IsBegin();
    bool IsEnd();

    void MoveToNextCycle();
    void MoveToPreviousCycle();

private:
    void UpdateCycleData();

    CycleHeader GetCurrentCycleHeader();
    CycleFooter GetPreviousCycleFooter();

    void CheckOffset(int64_t offset);

    std::ifstream* m_pStream{ nullptr };

    int64_t m_FileSize{ 0 };
    int64_t m_Offset{ 0 };

    char* m_pCycleData{ nullptr };
    int64_t m_CycleDataSize{ 0 };
};

}}
