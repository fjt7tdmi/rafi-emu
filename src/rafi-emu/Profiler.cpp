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

#include <iostream>

#ifdef WIN32
#include <Windows.h>
#else
#include <time.h>
#endif

#include <rafi/emu.h>

#include "Profiler.h"

namespace rafi { namespace emu {

Profiler::Profiler()
{
    for (int i = 0; i < Phase_MaxCount; i++)
    {
        m_Times[i] = 0;
    }
}

void Profiler::Switch(Phase phase)
{
    if (!m_Enabled)
    {
        return;
    }

    if (!(Phase_None <= phase && phase < Phase_MaxCount))
    {
        RAFI_EMU_NOT_IMPLEMENTED();
    }

    const auto prevPhase = m_CurrentPhase;
    const auto prevPhaseStart = m_CurrentPhaseStart;

    m_CurrentPhase = phase;

#ifdef WIN32
#pragma warning (disable: 4996)
    LARGE_INTEGER tick;
    QueryPerformanceCounter(&tick);

    m_CurrentPhaseStart = static_cast<int64_t>(tick.QuadPart);
#else
    m_CurrentPhaseStart = static_cast<int64_t>(clock());
#endif

    m_Times[prevPhase] += (m_CurrentPhaseStart - prevPhaseStart);
}

void Profiler::Dump()
{
    if (!m_Enabled)
    {
        return;
    }

#ifdef WIN32
    const double divisor = 1;
#else
    const double divisor = CLOCKS_PER_SEC;
#endif

    std::cout << std::dec << "Profiler" << std::endl
        << "\tcpu:     " << m_Times[Phase_Cpu] / divisor << std::endl
        << "\tfetch:   " << m_Times[Phase_Fetch] / divisor << std::endl
        << "\tdecode:  " << m_Times[Phase_Decode] / divisor << std::endl
        << "\texecute: " << m_Times[Phase_Execute] / divisor << std::endl
        << "\tio:      " << m_Times[Phase_Io] / divisor << std::endl
        << "\tdump:    " << m_Times[Phase_Dump] / divisor << std::endl;
}

void Profiler::Enable()
{
    m_Enabled = true;
}

}}
