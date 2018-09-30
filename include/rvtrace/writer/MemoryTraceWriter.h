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

#include <cstdio>

#include "ITraceWriter.h"

namespace rvtrace {

class MemoryTraceWriterImpl;

class MemoryTraceWriter : ITraceWriter
{
public:
    MemoryTraceWriter(void* buffer, int64_t size);
    virtual ~MemoryTraceWriter();

    virtual void Write(void* buffer, int64_t size);

private:
    MemoryTraceWriterImpl* m_pImpl;
};

}
