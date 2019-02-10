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

template<typename BaseInteger, int ExponentWidth, int FractionWidth>
class FpBase : public BitField<BaseInteger>
{
public:
    explicit FpBase(BaseInteger value)
        : BitField<BaseInteger>(value)
    {
    }

    uint32_t GetSign() const
    {
        return GetMember<Sign>();
    }

    uint32_t GetExponent() const
    {
        return GetMember<Exponent>();
    }

    uint32_t GetFraction() const
    {
        return GetMember<Fraction>();
    }

    bool IsZero() const
    {
        return GetExponent() == 0 && GetFraction() == 0;
    }

    bool IsPositiveZero() const
    {
        return IsZero() && GetSign() == 0;
    }

    bool IsNegativeZero() const
    {
        return IsZero() && GetSign() == 1;
    }

    bool IsNan() const
    {
        return GetExponent() == ((1 << ExponentWidth) - 1) && GetFraction() != 0;
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
    using Sign = BitFieldMember<BaseInteger, FractionWidth + ExponentWidth>;
    using Exponent = BitFieldMember<BaseInteger, FractionWidth + ExponentWidth - 1, FractionWidth>;
    using Fraction = BitFieldMember<BaseInteger, FractionWidth - 1, 0>;
    using FractionMsb = BitFieldMember<BaseInteger, FractionWidth - 1>;
};

class Fp32 : public FpBase<uint32_t, 8, 23>
{
public:
    explicit Fp32(uint32_t value)
        : FpBase<uint32_t, 8, 23>(value)
    {
    }

    static const uint32_t CanonicalQuietNan = 0x7fc00000;
    static const uint32_t CanonicalSignalingNan = 0x7f800001;
};

class Fp64 : public FpBase<uint64_t, 11, 52>
{
public:
    explicit Fp64(uint64_t value)
        : FpBase<uint64_t, 11, 52>(value)
    {
    }

    static const uint64_t CanonicalQuietNan = 0x7ff8000000000000ull;
    static const uint64_t CanonicalSignalingNan = 0x7ff0000000000001ull;
};

float32_t ToFloat32(uint32_t value)
{
    float32_t result;
    result.v = value;
    return result;
}

float64_t ToFloat64(uint64_t value)
{
    float64_t result;
    result.v = value;
    return result;
}

float32_t Negate(float32_t value)
{
    float32_t result;
    result.v = value.v ^ 0x80000000;
    return result;
}

}

uint32_t Add(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;
    
    return f32_add(ToFloat32(x), ToFloat32(y)).v;
}

uint32_t Sub(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;
    
    return f32_sub(ToFloat32(x), ToFloat32(y)).v;
}

uint32_t Mul(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;

    return f32_mul(ToFloat32(x), ToFloat32(y)).v;
}

uint32_t Div(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;

    return f32_div(ToFloat32(x), ToFloat32(y)).v;
}

uint32_t Sqrt(uint32_t x)
{
    softfloat_exceptionFlags = 0;

    return f32_sqrt(ToFloat32(x)).v;
}

bool Eq(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;

    return f32_eq(ToFloat32(x), ToFloat32(y));
}

bool Le(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;

    return f32_le(ToFloat32(x), ToFloat32(y));
}

bool Lt(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;

    return f32_lt(ToFloat32(x), ToFloat32(y));
}

uint32_t Min(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;

    const auto cmpResult = f32_le_quiet(ToFloat32(x), ToFloat32(y)) ? x : y;

    if (Fp32(x).IsZero() && Fp32(y).IsZero())
    {
        if (Fp32(x).IsPositiveZero() && Fp32(y).IsNegativeZero())
        {
            return y;
        }
        else
        {
            return x;
        }
    }
    else if (Fp32(x).IsNan() && Fp32(y).IsNan())
    {
        if (Fp32(x).IsSignalingNan() || Fp32(y).IsSignalingNan())
        {
            return Fp32::CanonicalSignalingNan;
        }
        else
        {
            return Fp32::CanonicalQuietNan;
        }
    }
    else if (!Fp32(x).IsNan() && Fp32(y).IsNan())
    {
        return x;
    }
    else if (Fp32(x).IsNan() && !Fp32(y).IsNan())
    {
        return y;
    }
    else
    {
        return cmpResult;
    }
}

uint32_t Max(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;

    const auto cmpResult = f32_le_quiet(ToFloat32(x), ToFloat32(y)) ? y : x;

    if (Fp32(x).IsZero() && Fp32(y).IsZero())
    {
        if (Fp32(x).IsPositiveZero() && Fp32(y).IsNegativeZero())
        {
            return x;
        }
        else
        {
            return y;
        }
    }
    else if (Fp32(x).IsNan() && Fp32(y).IsNan())
    {
        if (Fp32(x).IsSignalingNan() || Fp32(y).IsSignalingNan())
        {
            return Fp32::CanonicalSignalingNan;
        }
        else
        {
            return Fp32::CanonicalQuietNan;
        }
    }
    else if (!Fp32(x).IsNan() && Fp32(y).IsNan())
    {
        return x;
    }
    else if (Fp32(x).IsNan() && !Fp32(y).IsNan())
    {
        return y;
    }
    else
    {
        return cmpResult;
    }
}

uint32_t MulAdd(uint32_t x, uint32_t y, uint32_t z)
{
    softfloat_exceptionFlags = 0;

    const auto tmp = f32_mul(ToFloat32(x), ToFloat32(y));
    return f32_add(tmp, ToFloat32(z)).v;
}

uint32_t MulSub(uint32_t x, uint32_t y, uint32_t z)
{
    softfloat_exceptionFlags = 0;

    const auto tmp = f32_mul(ToFloat32(x), ToFloat32(y));
    return f32_sub(tmp, ToFloat32(z)).v;
}

uint32_t NegMulAdd(uint32_t x, uint32_t y, uint32_t z)
{
    softfloat_exceptionFlags = 0;

    const auto tmp = Negate(f32_mul(ToFloat32(x), ToFloat32(y)));
    return f32_sub(tmp, ToFloat32(z)).v;
}

uint32_t NegMulSub(uint32_t x, uint32_t y, uint32_t z)
{
    softfloat_exceptionFlags = 0;

    const auto tmp = Negate(f32_mul(ToFloat32(x), ToFloat32(y)));
    return f32_add(tmp, ToFloat32(z)).v;
}

int32_t FloatToInt32(uint32_t x, int roundMode)
{
    softfloat_exceptionFlags = 0;

    return f32_to_i32(ToFloat32(x), roundMode, true);
}

uint32_t FloatToUInt32(uint32_t x, int roundMode)
{
    softfloat_exceptionFlags = 0;

    return f32_to_ui32(ToFloat32(x), roundMode, true);
}

uint32_t Int32ToFloat(int32_t x)
{
    softfloat_exceptionFlags = 0;

    return i32_to_f32(x).v;
}

uint32_t UInt32ToFloat(uint32_t x)
{
    softfloat_exceptionFlags = 0;

    return ui32_to_f32(x).v;
}

uint32_t ConvertToRvFpClass(uint32_t x)
{
    Fp32 f(x);

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
    return static_cast<int>(softfloat_exceptionFlags);
}

void SetRvExceptionFlags(int rvFlags)
{
    softfloat_exceptionFlags = static_cast<decltype(softfloat_exceptionFlags)>(rvFlags);
}

int GetRvRoundMode()
{
    return static_cast<int>(softfloat_roundingMode);
}

void SetRvRoundMode(int mode)
{
    softfloat_roundingMode = static_cast<decltype(softfloat_roundingMode)>(mode);
}

}}
