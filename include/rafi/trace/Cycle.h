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

#include <stdint.h>

#include <rafi/trace/CycleReader.h>

namespace rafi { namespace trace {

class CycleImpl;

class Cycle
{
public:
    Cycle();
    Cycle(const void* buffer, size_t bufferSize);

    ~Cycle();

    const IoNode* GetIoNode() const;

private:
    void* m_pBuffer{ nullptr };
    size_t m_BufferSize{ 0 };

    CycleReader* m_pView{ nullptr };
};

}}
