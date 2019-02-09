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

#include <cfenv>
#include <rafi/fp.h>

extern "C"
{
#include <softfloat.h>
}

namespace rafi { namespace fp {

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
        return GetExponent() == 255 && GetFraction() != 0;
    }

    using Sign = BitFieldMember<31>;
    using Exponent = BitFieldMember<30, 23>;
    using Fraction = BitFieldMember<22, 0>;
    using FractionMsb = BitFieldMember<22>;
};

float32 ToFloat32(uint32_t value)
{
    return static_cast<float32>(value);
}

float64 ToFloat64(uint64_t value)
{
    return static_cast<float64>(value);
}

uint32_t ToUInt32(float32 value)
{
    return static_cast<uint32_t>(value);
}

uint64_t ToUInt64(float64 value)
{
    return static_cast<uint64_t>(value);
}

uint32_t AdjustNan(uint32_t value)
{
    Float f(value);

    if (f.IsQuietNan())
    {
        return 0x7fc00000; // for double, use 0d:7ff8000000000000
    }
    else if (f.IsSignalingNan())
    {
        return 0x7f800001; // for double, use 0d:7ff0000000000001
    }
    else
    {
        return value;
    }
}

}

uint32_t Add(uint32_t x, uint32_t y)
{
    float_exception_flags = 0;
    
    const auto result = ToUInt32(float32_add(ToFloat32(x), ToFloat32(y)));

    return AdjustNan(result);
}

uint32_t Sub(uint32_t x, uint32_t y)
{
    float_exception_flags = 0;
    
    const auto result = ToUInt32(float32_sub(ToFloat32(x), ToFloat32(y)));

    return AdjustNan(result);
}

uint32_t Mul(uint32_t x, uint32_t y)
{
    float_exception_flags = 0;

    const auto result = ToUInt32(float32_mul(ToFloat32(x), ToFloat32(y)));

    return AdjustNan(result);
}

uint32_t Div(uint32_t x, uint32_t y)
{
    float_exception_flags = 0;

    const auto result = ToUInt32(float32_div(ToFloat32(x), ToFloat32(y)));

    return AdjustNan(result);
}

uint32_t Sqrt(uint32_t x)
{
    float_exception_flags = 0;

    const auto result = ToUInt32(float32_sqrt(ToFloat32(x)));

    return AdjustNan(result);
}

int Eq(uint32_t x, uint32_t y)
{
    float_exception_flags = 0;

    const auto result = float32_eq(ToFloat32(x), ToFloat32(y));

    return AdjustNan(result);
}

int Le(uint32_t x, uint32_t y)
{
    float_exception_flags = 0;

    const auto result = float32_le(ToFloat32(x), ToFloat32(y));

    return AdjustNan(result);
}

int Lt(uint32_t x, uint32_t y)
{
    float_exception_flags = 0;

    const auto result = float32_lt(ToFloat32(x), ToFloat32(y));

    return AdjustNan(result);
}

int32_t FloatToInt32(uint32_t x)
{
    float_exception_flags = 0;

    return float32_to_int32(ToFloat32(x));
}

uint32_t FloatToUInt32(uint32_t x)
{
    float_exception_flags = 0;

    return float32_to_int32(ToFloat32(x));
}

uint32_t Int32ToFloat(int32_t x)
{
    float_exception_flags = 0;

    const auto result = ToUInt32(int32_to_float32(x));

    return AdjustNan(result);
}

uint32_t UInt32ToFloat(uint32_t x)
{
    float_exception_flags = 0;

    const auto result = ToUInt32(int32_to_float32(static_cast<int32_t>(x)));

    return AdjustNan(result);
}

uint32_t ConvertToRvFpClass(uint32_t x)
{
    Float f(x);

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

int GetRvExceptionFlags()
{
    uint32_t rvFlags = 0;

    if (float_exception_flags & float_flag_inexact)
    {
        rvFlags |= 0x01;
    }
    if (float_exception_flags & float_flag_underflow)
    {
        rvFlags |= 0x02;
    }
    if (float_exception_flags & float_flag_overflow)
    {
        rvFlags |= 0x04;
    }
    if (float_exception_flags & float_flag_divbyzero)
    {
        rvFlags |= 0x08;
    }
    if (float_exception_flags & float_flag_invalid)
    {
        rvFlags |= 0x10;
    }

    return rvFlags;
}

void SetRvExceptionFlags(int rvFlags)
{
    int8_t flags = 0;

    if (rvFlags & 0x01)
    {
        flags |= float_flag_inexact;
    }
    if (rvFlags & 0x02)
    {
        flags |= float_flag_underflow;
    }
    if (rvFlags & 0x04)
    {
        flags |= float_flag_overflow;
    }
    if (rvFlags & 0x08)
    {
        flags |= float_flag_divbyzero;
    }
    if (rvFlags & 0x10)
    {
        flags |= float_flag_invalid;
    }

    float_exception_flags = flags;
}

int GetRvRoundMode()
{
    switch(float_rounding_mode)
    {
    case float_round_nearest_even:
        // Round to Nearest, ties to even
        return 0;
    case float_round_to_zero:
        return 1;
    case float_round_down:
        return 2;
    case float_round_up:
        return 3;
    default:
        RAFI_NOT_IMPLEMENTED();
    }
}

void SetRvRoundMode(int mode)
{
    switch(mode)
    {
    case 0:
        // Round to Nearest, ties to even
        float_rounding_mode = float_round_nearest_even;
        break;
    case 1:
        float_rounding_mode = float_round_to_zero;
        break;
    case 2:
        float_rounding_mode = float_round_down;
        break;
    case 3:
        float_rounding_mode = float_round_up;
        break;
    case 4:
        // Round to Nearest, ties to max magnitude
        float_rounding_mode = float_round_nearest_even;
        break;
    default:
        RAFI_NOT_IMPLEMENTED();
    }
}

}}
