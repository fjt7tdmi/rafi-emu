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

class TraceLogger final
{
public:
    TraceLogger(XLEN xlen, const char* path, const System* pSystem);
    ~TraceLogger();

    void EnableDump();
    void EnableDumpCsr();
    void EnableDumpFpReg();
    void EnableDumpIntReg();
    void EnableDumpMemory();
    void EnableDumpHostIo();

    void RecordState();
    void DumpCycle(int cycle);

private:
    XLEN m_XLEN;
    const char* m_pPath;
    const System* m_pSystem;

    rafi::trace::ITraceWriter* m_pTraceWriter;

    // State
    trace::NodeIntReg32 m_IntReg32;
    trace::NodeIntReg64 m_IntReg64;
    trace::NodeFpReg m_FpReg;
    trace::NodeIo m_Io;

    bool m_StateRecorded {false};

    bool m_Enabled {false};
    bool m_EnableDumpCsr {false};
    bool m_EnableDumpFpReg {false};
    bool m_EnableDumpIntReg {false};
    bool m_EnableDumpMemory {false};
    bool m_EnableDumpHostIo {false};
};

}}
