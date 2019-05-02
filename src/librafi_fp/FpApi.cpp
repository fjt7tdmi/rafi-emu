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
#include <rafi/common.h>
#include <rafi/fp.h>

extern "C"
{
#include <softfloat.h>
}

namespace rafi { namespace fp {

namespace {

// ----------------------------------------------------------------------------
// Fp classes

template<typename BaseInteger, int ExponentWidth, int FractionWidth>
class FpBase : public BitField<BaseInteger>
{
public:
    explicit FpBase(BaseInteger value)
        : BitField<BaseInteger>(value)
    {
    }

    BaseInteger GetSign() const
    {
        return this->template GetMember<Sign>();
    }

    BaseInteger GetExponent() const
    {
        return this->template GetMember<Exponent>();
    }

    BaseInteger GetFraction() const
    {
        return this->template GetMember<Fraction>();
    }

    bool IsZero() const
    {
        return GetExponent() == BaseInteger(0) && GetFraction() == BaseInteger(0);
    }

    bool IsInf() const
    {
        return GetExponent() == MaxExponent && GetFraction() == BaseInteger(0);
    }

    bool IsNormal() const
    {
        return 1 <= GetExponent() && GetExponent() < MaxExponent;
    }

    bool IsSubNormal() const
    {
        return GetExponent() == BaseInteger(0) && GetFraction() != BaseInteger(0);
    }

    bool IsNan() const
    {
        return GetExponent() == MaxExponent && GetFraction() != BaseInteger(0);
    }

    bool IsQuietNan() const
    {
        return IsNan() && this->template GetMember<FractionMsb>() != BaseInteger(0);
    }

    bool IsSignalingNan() const
    {
        return IsNan() && this->template GetMember<FractionMsb>() == BaseInteger(0);
    }

private:
    using Sign = BitFieldMember<BaseInteger, FractionWidth + ExponentWidth>;
    using Exponent = BitFieldMember<BaseInteger, FractionWidth + ExponentWidth - 1, FractionWidth>;
    using Fraction = BitFieldMember<BaseInteger, FractionWidth - 1, 0>;
    using FractionMsb = BitFieldMember<BaseInteger, FractionWidth - 1>;

    static const BaseInteger MaxExponent = (BaseInteger(1) << ExponentWidth) - BaseInteger(1);
};

class Fp32 : public FpBase<uint32_t, 8, 23>
{
public:
    explicit Fp32(uint32_t value)
        : FpBase<uint32_t, 8, 23>(value)
    {
    }

    static Fp32 GetCanonicalQuietNan()
    {
        return Fp32(0x7fc00000);
    }

    static Fp32 GetCanonicalSignalingNan()
    {
        return Fp32(0x7f800001);
    }
};

class Fp64 : public FpBase<uint64_t, 11, 52>
{
public:
    explicit Fp64(uint64_t value)
        : FpBase<uint64_t, 11, 52>(value)
    {
    }

    static Fp64 GetCanonicalQuietNan()
    {
        return Fp64(0x7ff8000000000000ull);
    }

    static Fp64 GetCanonicalSignalingNan()
    {
        return Fp64(0x7ff0000000000001ull);
    }
};

// ----------------------------------------------------------------------------
// float32_t, float64_t utilities

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
    result.v = value.v ^ (1ul << 31);
    return result;
}

float64_t Negate(float64_t value)
{
    float64_t result;
    result.v = value.v ^ (1ull << 63);
    return result;
}

// ----------------------------------------------------------------------------
// min/max utilities

bool LeQuiet(const Fp32& x, const Fp32& y)
{
    return f32_le_quiet(ToFloat32(x.GetValue()), ToFloat32(y.GetValue()));
}

bool LeQuiet(const Fp64& x, const Fp64& y)
{
    return f64_le_quiet(ToFloat64(x.GetValue()), ToFloat64(y.GetValue()));
}

template<typename T>
T MinImpl(const T& x, const T& y)
{
    const auto cmpResult = LeQuiet(x, y) ? x : y;

    if (x.IsZero() && x.IsZero())
    {
        if (x.GetSign() == 1 && y.GetSign() == 0)
        {
            return x;
        }
        else
        {
            return y;
        }
    }
    else if (x.IsNan() && y.IsNan())
    {
        if (x.IsSignalingNan() || y.IsSignalingNan())
        {
            return T::GetCanonicalSignalingNan();
        }
        else
        {
            return T::GetCanonicalQuietNan();
        }
    }
    else if (!x.IsNan() && y.IsNan())
    {
        return x;
    }
    else if (x.IsNan() && !y.IsNan())
    {
        return y;
    }
    else
    {
        return cmpResult;
    }
}

template<typename T>
T MaxImpl(const T& x, const T& y)
{
    const auto cmpResult = LeQuiet(x, y) ? y : x;

    if (x.IsZero() && y.IsZero())
    {
        if (x.GetSign() == 0 && y.GetSign() == 1)
        {
            return x;
        }
        else
        {
            return y;
        }
    }
    else if (x.IsNan() && y.IsNan())
    {
        if (x.IsSignalingNan() || y.IsSignalingNan())
        {
            return T::GetCanonicalSignalingNan();
        }
        else
        {
            return T::GetCanonicalQuietNan();
        }
    }
    else if (!x.IsNan() && y.IsNan())
    {
        return x;
    }
    else if (x.IsNan() && !y.IsNan())
    {
        return y;
    }
    else
    {
        return cmpResult;
    }
}

// ----------------------------------------------------------------------------

} // namespace

uint32_t Add(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;
    return f32_add(ToFloat32(x), ToFloat32(y)).v;
}

uint64_t Add(uint64_t x, uint64_t y)
{
    softfloat_exceptionFlags = 0;
    return f64_add(ToFloat64(x), ToFloat64(y)).v;
}

uint32_t Sub(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;
    return f32_sub(ToFloat32(x), ToFloat32(y)).v;
}

uint64_t Sub(uint64_t x, uint64_t y)
{
    softfloat_exceptionFlags = 0;
    return f64_sub(ToFloat64(x), ToFloat64(y)).v;
}

uint32_t Mul(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;
    return f32_mul(ToFloat32(x), ToFloat32(y)).v;
}

uint64_t Mul(uint64_t x, uint64_t y)
{
    softfloat_exceptionFlags = 0;
    return f64_mul(ToFloat64(x), ToFloat64(y)).v;
}

uint32_t Div(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;
    return f32_div(ToFloat32(x), ToFloat32(y)).v;
}

uint64_t Div(uint64_t x, uint64_t y)
{
    softfloat_exceptionFlags = 0;
    return f64_div(ToFloat64(x), ToFloat64(y)).v;
}

uint32_t Sqrt(uint32_t x)
{
    softfloat_exceptionFlags = 0;
    return f32_sqrt(ToFloat32(x)).v;
}

uint64_t Sqrt(uint64_t x)
{
    softfloat_exceptionFlags = 0;
    return f64_sqrt(ToFloat64(x)).v;
}

bool Eq(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;
    return f32_eq(ToFloat32(x), ToFloat32(y));
}

bool Eq(uint64_t x, uint64_t y)
{
    softfloat_exceptionFlags = 0;
    return f64_eq(ToFloat64(x), ToFloat64(y));
}

bool Le(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;
    return f32_le(ToFloat32(x), ToFloat32(y));
}

bool Le(uint64_t x, uint64_t y)
{
    softfloat_exceptionFlags = 0;
    return f64_le(ToFloat64(x), ToFloat64(y));
}

bool Lt(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;
    return f32_lt(ToFloat32(x), ToFloat32(y));
}

bool Lt(uint64_t x, uint64_t y)
{
    softfloat_exceptionFlags = 0;
    return f64_lt(ToFloat64(x), ToFloat64(y));
}

uint32_t Min(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;
    return MinImpl(Fp32(x), Fp32(y)).GetValue();
}

uint64_t Min(uint64_t x, uint64_t y)
{
    softfloat_exceptionFlags = 0;
    return MinImpl(Fp64(x), Fp64(y)).GetValue();
}

uint32_t Max(uint32_t x, uint32_t y)
{
    softfloat_exceptionFlags = 0;
    return MaxImpl(Fp32(x), Fp32(y)).GetValue();
}

uint64_t Max(uint64_t x, uint64_t y)
{
    softfloat_exceptionFlags = 0;
    return MaxImpl(Fp64(x), Fp64(y)).GetValue();
}

uint32_t MulAdd(uint32_t x, uint32_t y, uint32_t z)
{
    softfloat_exceptionFlags = 0;
    return f32_add(f32_mul(ToFloat32(x), ToFloat32(y)), ToFloat32(z)).v;
}

uint64_t MulAdd(uint64_t x, uint64_t y, uint64_t z)
{
    softfloat_exceptionFlags = 0;
    return f64_add(f64_mul(ToFloat64(x), ToFloat64(y)), ToFloat64(z)).v;
}

uint32_t MulSub(uint32_t x, uint32_t y, uint32_t z)
{
    softfloat_exceptionFlags = 0;
    return f32_sub(f32_mul(ToFloat32(x), ToFloat32(y)), ToFloat32(z)).v;
}

uint64_t MulSub(uint64_t x, uint64_t y, uint64_t z)
{
    softfloat_exceptionFlags = 0;
    return f64_sub(f64_mul(ToFloat64(x), ToFloat64(y)), ToFloat64(z)).v;
}

uint32_t NegMulAdd(uint32_t x, uint32_t y, uint32_t z)
{
    softfloat_exceptionFlags = 0;
    return f32_sub(Negate(f32_mul(ToFloat32(x), ToFloat32(y))), ToFloat32(z)).v;
}

uint64_t NegMulAdd(uint64_t x, uint64_t y, uint64_t z)
{
    softfloat_exceptionFlags = 0;
    return f64_sub(Negate(f64_mul(ToFloat64(x), ToFloat64(y))), ToFloat64(z)).v;
}

uint32_t NegMulSub(uint32_t x, uint32_t y, uint32_t z)
{
    softfloat_exceptionFlags = 0;
    return f32_add(Negate(f32_mul(ToFloat32(x), ToFloat32(y))), ToFloat32(z)).v;
}

uint64_t NegMulSub(uint64_t x, uint64_t y, uint64_t z)
{
    softfloat_exceptionFlags = 0;
    return f64_add(Negate(f64_mul(ToFloat64(x), ToFloat64(y))), ToFloat64(z)).v;
}

uint32_t UnboxFloat(uint64_t x)
{
    const bool isUnboxed = (static_cast<uint32_t>(x >> 32) == 0xffffffff);
    if (isUnboxed)
    {
        return static_cast<uint32_t>(x);
    }
    else
    {
        return Fp32::GetCanonicalQuietNan().GetValue();
    }
}

int32_t DoubleToInt32(uint64_t x, int roundMode)
{
    softfloat_exceptionFlags = 0;
    return f64_to_i32(ToFloat64(x), roundMode, true);
}

int64_t DoubleToInt64(uint64_t x, int roundMode)
{
    softfloat_exceptionFlags = 0;
    return f64_to_i64(ToFloat64(x), roundMode, true);
}

uint32_t DoubleToUInt32(uint64_t x, int roundMode)
{
    softfloat_exceptionFlags = 0;
    return f64_to_ui32(ToFloat64(x), roundMode, true);
}

uint64_t DoubleToUInt64(uint64_t x, int roundMode)
{
    softfloat_exceptionFlags = 0;
    return f64_to_ui64(ToFloat64(x), roundMode, true);
}

int32_t FloatToInt32(uint32_t x, int roundMode)
{
    softfloat_exceptionFlags = 0;
    return f32_to_i32(ToFloat32(x), roundMode, true);
}

int64_t FloatToInt64(uint32_t x, int roundMode)
{
    softfloat_exceptionFlags = 0;
    return f32_to_i64(ToFloat32(x), roundMode, true);
}

uint32_t FloatToUInt32(uint32_t x, int roundMode)
{
    softfloat_exceptionFlags = 0;
    return f32_to_ui32(ToFloat32(x), roundMode, true);
}

uint64_t FloatToUInt64(uint32_t x, int roundMode)
{
    softfloat_exceptionFlags = 0;
    return f32_to_ui64(ToFloat32(x), roundMode, true);
}

uint64_t Int32ToDouble(int32_t x)
{
    softfloat_exceptionFlags = 0;
    return i32_to_f64(x).v;
}

uint64_t Int64ToDouble(int64_t x)
{
    softfloat_exceptionFlags = 0;
    return i64_to_f64(x).v;
}

uint64_t UInt32ToDouble(uint32_t x)
{
    softfloat_exceptionFlags = 0;
    return ui32_to_f64(x).v;
}

uint64_t UInt64ToDouble(uint64_t x)
{
    softfloat_exceptionFlags = 0;
    return ui64_to_f64(x).v;
}

uint32_t Int32ToFloat(int32_t x)
{
    softfloat_exceptionFlags = 0;
    return i32_to_f32(x).v;
}

uint32_t Int64ToFloat(int64_t x)
{
    softfloat_exceptionFlags = 0;
    return i64_to_f32(x).v;
}

uint32_t UInt32ToFloat(uint32_t x)
{
    softfloat_exceptionFlags = 0;
    return ui32_to_f32(x).v;
}

uint32_t UInt64ToFloat(uint64_t x)
{
    softfloat_exceptionFlags = 0;
    return ui64_to_f32(x).v;
}

uint32_t DoubleToFloat(uint64_t x)
{
    softfloat_exceptionFlags = 0;
    return f64_to_f32(ToFloat64(x)).v;
}

uint64_t FloatToDouble(uint32_t x)
{
    softfloat_exceptionFlags = 0;
    return f32_to_f64(ToFloat32(x)).v;
}

namespace {

template <typename T>
uint32_t ConvertToRvFpClassImpl(const T& x)
{
    if (x.GetSign() == 1 && x.IsInf())
    {
        return 1 << 0; // negative infinity
    }
    else if (x.GetSign() == 1 && x.IsNormal())
    {
        return 1 << 1; // negative normal
    }
    else if (x.GetSign() == 1 && x.IsSubNormal())
    {
        return 1 << 2; // negative subnormal
    }
    else if (x.GetSign() == 1 && x.IsZero())
    {
        return 1 << 3; // negative zero
    }
    else if (x.GetSign() == 0 && x.IsZero())
    {
        return 1 << 4; // positive zero
    }
    else if (x.GetSign() == 0 && x.IsSubNormal())
    {
        return 1 << 5; // positive subnormal
    }
    else if (x.GetSign() == 0 && x.IsNormal())
    {
        return 1 << 6; // positive normal
    }
    else if (x.GetSign() == 0 && x.IsInf())
    {
        return 1 << 7; // positive infinity
    }
    else if (x.IsSignalingNan())
    {
        return 1 << 8; // signaling NaN
    }
    else
    {
        return 1 << 9; // quiet NaN
    }
}

} // namespace

uint32_t ConvertToRvFpClass(uint32_t x)
{
    return ConvertToRvFpClassImpl(Fp32(x));
}

uint32_t ConvertToRvFpClass(uint64_t x)
{
    return ConvertToRvFpClassImpl(Fp64(x));
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
