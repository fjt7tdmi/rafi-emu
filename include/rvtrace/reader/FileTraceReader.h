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

#include "ITraceReader.h"
#include "MemoryTraceReader.h"

namespace rvtrace {

class FileTraceReader : public ITraceReader
{
public:
    FileTraceReader(const char* path);
    virtual ~FileTraceReader();

    virtual const void* GetCurrentCycleData();
    virtual int64_t GetCurrentCycleDataSize();

    virtual bool IsBegin();
    virtual bool IsEnd();

    virtual void MoveToNextCycle();
    virtual void MoveToPreviousCycle();

private:
    MemoryTraceReader* m_pMemoryTraceReader;

    char* m_pBuffer;
    int64_t m_BufferSize;
};

}