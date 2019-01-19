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

#include <Exception.h>

#include "FpUtil.h"

namespace rafi { namespace emu {

ScopedFpRound::ScopedFpRound(int rvRound);
{
    m_OriginalHostRound = std::fegetround();

    const auto hostRound = ConvertToHostRoundingMode(rvRound);

    std::fesetround(hostRound);
}

ScopedFpRound::~ScopedFpRound();
{
    std::fesetround(m_Original);
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
        throw NotImplementedException();
    }
}

uint32_t GetRvFpExceptFlags()
{
    std::fexcept_t hostFlags = 0;
    std::fegetexceptflag(&hostFlags, FE_ALL_EXCEPT);

    uint32_t rvFlags = 0;

    if (hostFlags & FE_INEXACT)
    {
        rvFlags |= 0x01;
    }
    if (hostFlags & FE_UNDERFLOW)
    {
        rvFlags |= 0x02;
    }
    if (hostFlags & FE_OVERFLOW)
    {
        rvFlags |= 0x04;
    }
    if (hostFlags & FE_DIVBYZERO)
    {
        rvFlags |= 0x08;
    }
    if (hostFlags & FE_INVALID)
    {
        rvFlags |= 0x10;
    }

    return rvFlags;
}

}}
