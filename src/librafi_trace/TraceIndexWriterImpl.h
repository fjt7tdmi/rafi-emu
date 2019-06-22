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

#include <rafi/trace.h>

namespace rafi { namespace trace {

class TraceIndexWriterImpl final
{
public:
    explicit TraceIndexWriterImpl(const char* pathBase);
    ~TraceIndexWriterImpl();

    void Write(void* buffer, int64_t size);

private:
    static const int MaxCycleCount = 10000;

    void OpenBinaryFile();
    void CloseBinaryFile();

    std::FILE* m_pBinaryFile{ nullptr };
    std::FILE* m_pIndexFile{ nullptr };

    int m_BinaryFileCount{ 0 };
    int m_CycleCount{ 0 };

    std::string m_PathBase;
};

}}
