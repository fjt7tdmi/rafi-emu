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
#include <string>
#include <vector>

#include <rafi/trace.h>

#include "GdbCycle.h"

namespace rafi { namespace trace {

class GdbTrace final
{
public:
    GdbTrace(std::basic_istream<char>* pInput);
    ~GdbTrace();

    const ICycle* GetCycle() const;

    bool IsEnd() const;

    void Next();

private:
    void UpdateGdbCycle();

    XLEN m_XLEN;

    std::basic_istream<char>* m_pInput;

    std::unique_ptr<GdbCycle> m_pGdbCycle;
};

}}
