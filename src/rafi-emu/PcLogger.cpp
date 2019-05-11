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

#include <cstdio>

#include <rafi/trace.h>

#include "PcLogger.h"

using namespace rafi::trace;

namespace rafi { namespace emu {

PcLogger::PcLogger(XLEN xlen, const char* path, const System* pSystem)
    : m_XLEN(xlen)
    , m_pPath(path)
    , m_pSystem(pSystem)
{
}

PcLogger::~PcLogger()
{
    if (m_pFile != nullptr)
    {
        std::fclose(m_pFile);
        m_pFile = nullptr;
    }
}

void PcLogger::EnableDump()
{
    if (m_Enabled)
    {
        return;
    }

    m_Enabled = true;
    m_pFile = std::fopen(m_pPath, "w");
}

void PcLogger::DumpCycle()
{
    if (!m_Enabled)
    {
        return;
    }

    if (!m_pSystem->IsOpEventExist())
    {
        return;
    }

    switch (m_XLEN)
    {
    case XLEN::XLEN32:
        DumpCycle32();
        break;
    case XLEN::XLEN64:
        DumpCycle64();
        break;
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }

}

void PcLogger::DumpCycle32()
{
    OpEvent opEvent;
    m_pSystem->CopyOpEvent(&opEvent);

    fprintf(m_pFile, "0x%08x\n", static_cast<unsigned int>(opEvent.virtualPc));
}

void PcLogger::DumpCycle64()
{
    OpEvent opEvent;
    m_pSystem->CopyOpEvent(&opEvent);

    fprintf(m_pFile, "0x%016llx\n", static_cast<unsigned long long>(opEvent.virtualPc));
}

}}
