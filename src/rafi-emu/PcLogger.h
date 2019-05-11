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

#include "System.h"

namespace rafi { namespace emu {

class PcLogger final
{
public:
    PcLogger(XLEN xlen, const char* path, const System* pSystem);
    ~PcLogger();

    void EnableDump();

    void DumpCycle();

private:
    void DumpCycle32();
    void DumpCycle64();

    XLEN m_XLEN;
    const char* m_pPath;
    const System* m_pSystem;

    std::FILE* m_pFile {nullptr};

    bool m_Enabled {false};
};

}}
