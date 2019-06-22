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

namespace rafi { namespace emu {

class Profiler
{
public:
    enum Phase
    {
        Phase_None = 0,
        Phase_Cpu,
        Phase_Fetch,
        Phase_Decode,
        Phase_Execute,
        Phase_Dump,
        Phase_Io,
        Phase_MaxCount,
    };

    Profiler();

    void Switch(Phase phase);
    void Dump();
    void Enable();

private:
    bool m_Enabled{ false };
    Phase m_CurrentPhase{ Phase_None};
    int64_t m_CurrentPhaseStart{ 0 };

    // Total time for each phase
    int64_t m_Times[Phase_MaxCount];
};

}}
