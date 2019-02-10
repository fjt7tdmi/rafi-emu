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

#include <iostream>

#include <rafi/common.h>

namespace rafi { namespace trace {

class TraceException
{
public:
    explicit TraceException(const char *pMessage)
        : m_pMessage(pMessage)
    {
    }

    TraceException(const char *pMessage, int64_t offset)
        : m_pMessage(pMessage)
        , m_Offset(offset)
        , m_OffsetValid(true)
    {
    }

    virtual void PrintMessage() const
    {
        if (m_OffsetValid)
        {
            std::cout << "[TraceException] " << m_pMessage << " (offset:0x" << std::hex << m_Offset << ")" << std::endl;
        }
        else
        {
            std::cout << "[TraceException] " << m_pMessage << std::endl;
        }
    }

private:
    const char* m_pMessage {nullptr};
    int64_t m_Offset {0};
    bool m_OffsetValid {false};
};

}}
