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

#include <rafi/Common.h>

#include "FpUtil.h"

namespace rafi { namespace emu {

namespace {

class Float : public BitField
{
public:
    explicit Float(uint32_t value)
        : BitField(value)
    {
    }

    int32_t GetSign() const
    {
        return GetMember<Sign>();
    }

    int32_t GetExponent() const
    {
        return GetMember<Exponent>();
    }

    int32_t GetFraction() const
    {
        return GetMember<Fraction>();
    }

    bool IsQuietNan() const
    {
        return IsNan() && GetMember<FractionMsb>() != 0;
    }

    bool IsSignalingNan() const
    {
        return IsNan() && GetMember<FractionMsb>() == 0;
    }

private:
    bool IsNan() const
    {
        return GetSign() == 0 && GetExponent() == 255 && GetFraction() != 0;
    }

    using Sign = BitFieldMember<31>;
    using Exponent = BitFieldMember<30, 23>;
    using Fraction = BitFieldMember<22, 0>;
    using FractionMsb = BitFieldMember<22>;
};

}

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
        RAFI_EMU_NOT_IMPLEMENTED();
    }
}

uint32_t GetRvFpExceptFlags(const std::fexcept_t& value)
{
    uint32_t rvFlags = 0;

    if (value & FE_INEXACT)
    {
        rvFlags |= 0x01;
    }
    if (value & FE_UNDERFLOW)
    {
        rvFlags |= 0x02;
    }
    if (value & FE_OVERFLOW)
    {
        rvFlags |= 0x04;
    }
    if (value & FE_DIVBYZERO)
    {
        rvFlags |= 0x08;
    }
    if (value & FE_INVALID)
    {
        rvFlags |= 0x10;
    }

    return rvFlags;
}

uint32_t GetRvFpClass(uint32_t value)
{
    Float f(value);

    if (f.GetSign() == 1 && f.GetExponent() == 255 && f.GetFraction() == 0)
    {
        return 1 << 0; // negative infinity
    }
    else if (f.GetSign() == 1 && 1 <= f.GetExponent() && f.GetExponent() <= 254)
    {
        return 1 << 1; // negative normal
    }
    else if (f.GetSign() == 1 && f.GetExponent() == 0 && f.GetFraction() != 0)
    {
        return 1 << 2; // negative subnormal
    }
    else if (f.GetSign() == 1 && f.GetExponent() == 0 && f.GetFraction() == 0)
    {
        return 1 << 3; // negative zero
    }
    else if (f.GetSign() == 0 && f.GetExponent() == 0 && f.GetFraction() == 0)
    {
        return 1 << 4; // positive zero
    }
    else if (f.GetSign() == 0 && f.GetExponent() == 0 && f.GetFraction() != 0)
    {
        return 1 << 5; // positive subnormal
    }
    else if (f.GetSign() == 0 && 1 <= f.GetExponent() && f.GetExponent() <= 254)
    {
        return 1 << 6; // positive normal
    }
    else if (f.GetSign() == 0 && f.GetExponent() == 255 && f.GetFraction() == 0)
    {
        return 1 << 7; // positive infinity
    }
    else
    {
        if (f.IsSignalingNan())
        {
            return 1 << 8; // signaling NaN
        }
        else
        {
            return 1 << 9; // quiet NaN
        }
    }
}

bool IsSignalingNan(uint32_t value)
{
    Float f(value);
    return f.IsSignalingNan();
}

bool IsQuietNan(uint32_t value)
{
    Float f(value);
    return f.IsQuietNan();
}

}}
