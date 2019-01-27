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
#endif

#include <rafi/emu.h>

#include "Profiler.h"

namespace rafi { namespace emu { namespace log {

#ifdef WIN32
#pragma warning (disable: 4996)

void Profiler::SwitchPhase(Phase phase)
{
    if (!(Phase_None <= phase && phase < Phase_MaxCount))
    {
        RAFI_EMU_NOT_IMPLEMENTED();
    }

    const auto prevPhase = m_CurrentPhase;
    const auto prevPhaseStart = m_CurrentPhaseStart;

    LARGE_INTEGER tick;
    QueryPerformanceCounter(&tick);

    m_CurrentPhase = phase;
    m_CurrentPhaseStart = static_cast<int64_t>(tick.QuadPart);

    m_Times[prevPhase] += (m_CurrentPhaseStart - prevPhaseStart);
}

void Profiler::Dump()
{
    std::cout << std::dec
        << "[Profiler] process=" << m_Times[Phase_Process]
        << ", dump=" << m_Times[Phase_Dump]
        << std::endl;
}

#else

void Profiler::SwitchPhase(Phase phase)
{
    (void)phase;
}

void Profiler::Dump()
{
    std::cout << "Profiler does not support this platform." << std::endl;
}

#endif

Profiler::Profiler()
    : m_CurrentPhase(Phase_None)
    , m_CurrentPhaseStart(0)
{
    for (int i = 0; i < Phase_MaxCount; i++)
    {
        m_Times[i] = 0;
    }
    SwitchPhase(Phase_None);
}

}}}
