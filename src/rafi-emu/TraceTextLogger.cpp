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

#include <cinttypes>
#include <cstdio>

#include <rafi/trace.h>

#include "TraceTextLogger.h"

using namespace rafi::trace;

namespace rafi { namespace emu {

TraceTextLogger::TraceTextLogger(XLEN xlen, const char* path, const System* pSystem)
    : m_XLEN(xlen)
    , m_pPath(path)
    , m_pSystem(pSystem)
{
}

TraceTextLogger::~TraceTextLogger()
{
    if (m_pFile != nullptr)
    {
        std::fflush(m_pFile);
        std::fclose(m_pFile);
        m_pFile = nullptr;
    }
}

void TraceTextLogger::EnableDump()
{
    if (m_Enabled)
    {
        return;
    }

    m_Enabled = true;
    m_pFile = std::fopen(m_pPath, "w");

    fprintf(m_pFile, "XLEN %s\n", m_XLEN == XLEN::XLEN32 ? "32" : "64");
}

void TraceTextLogger::DumpCycle(int cycle)
{
    if (!m_Enabled)
    {
        return;
    }

    switch (m_XLEN)
    {
    case XLEN::XLEN32:
        DumpCycle32(cycle);
        break;
    case XLEN::XLEN64:
        DumpCycle64(cycle);
        break;
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }

}

void TraceTextLogger::DumpCycle32(int cycle)
{
    const auto pc = m_pSystem->GetPc();

    IntReg32Node intReg;
    m_pSystem->CopyIntReg(&intReg);

    fprintf(m_pFile, "NOTE cycle%" PRId32 "\n", cycle);
    fprintf(m_pFile, "PC  %016" PRIx32 "  0\n", static_cast<uint32_t>(pc));
    fprintf(m_pFile, "INT\n");

    for (int i = 0; i < 32; i++) {
        fprintf(m_pFile, "  %08" PRIx32, intReg.regs[i]);
        if ((i & 3) == 3) {
            fprintf(m_pFile, "\n");
        }
    }

    fprintf(m_pFile, "BREAK\n");
}

void TraceTextLogger::DumpCycle64(int cycle)
{
    const auto pc = m_pSystem->GetPc();

    IntReg64Node intReg;
    m_pSystem->CopyIntReg(&intReg);

    fprintf(m_pFile, "NOTE cycle%" PRId32 "\n", cycle);
    fprintf(m_pFile, "PC  %016" PRIx64 "  0\n", static_cast<uint64_t>(pc));
    fprintf(m_pFile, "INT\n");

    for (int i = 0; i < 32; i++) {
        fprintf(m_pFile, "  %016" PRIx64, intReg.regs[i]);
        if ((i & 3) == 3) {
            fprintf(m_pFile, "\n");
        }
    }

    fprintf(m_pFile, "BREAK\n");
}

}}
