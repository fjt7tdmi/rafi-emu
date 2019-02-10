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

namespace rafi {

template <typename T, int msb, int lsb = msb>
class BitFieldMember
{
public:
    static const T Msb = msb;
    static const T Lsb = lsb;
    static const T Width = Msb - Lsb + 1u;
    static const T Mask = ((1u << Width) - 1u) << Lsb;
};

template <typename T>
class BitField
{
public:
    template <int msb, int lsb = msb>
    using Member = BitFieldMember<T, msb, lsb>;

    BitField()
    {
    }

    explicit BitField(T value)
    {
        m_Value = value;
    }

    operator T() const
    {
        return GetValue();
    }

    T GetValue() const
    {
        return m_Value;
    }

    BitField& SetValue(T value)
    {
        m_Value = value;
        return *this;
    }

    T GetWithMask(T mask) const
    {
        return (m_Value & mask);
    }

    BitField& SetWithMask(T value, T mask)
    {
        m_Value = (m_Value & ~mask) | (value & mask);
        return *this;
    }

    template <typename TMember>
    T GetMember() const
    {
        return (m_Value & TMember::Mask) >> TMember::Lsb;
    }

    template <typename TMember>
    BitField& SetMember(T value)
    {
        m_Value = (m_Value & ~TMember::Mask) | ((value << TMember::Lsb) & TMember::Mask);
        return *this;
    }

private:
    T m_Value{};
};

using BitField32 = BitField<uint32_t>;
using BitField64 = BitField<uint64_t>;

}
