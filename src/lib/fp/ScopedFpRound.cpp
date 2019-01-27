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

#pragma fenv_access (on)

#include <softfloat.h>

#include <rafi/fp.h>
#include <rafi/macro.h>

namespace rafi { namespace fp {

ScopedFpRound::ScopedFpRound(int rvRound)
{
    m_OriginalHostRound = std::fegetround();

    const auto hostRound = ConvertToHostRoundingMode(rvRound);

    std::fesetround(hostRound);
}

ScopedFpRound::~ScopedFpRound()
{
    std::fesetround(m_OriginalHostRound);
}

int ScopedFpRound::ConvertToHostRoundingMode(int rvRound)
{
    switch(rvRound)
    {
    case 0:
        // ties to even
        return FE_TONEAREST;
    case 1:
        return FE_TOWARDZERO;
    case 2:
        return FE_DOWNWARD;
    case 3:
        return FE_UPWARD;
    case 4:
        // ties to max magnitude
        return FE_TONEAREST;
    default:
        RAFI_NOT_IMPLEMENTED();
    }
}

}}
