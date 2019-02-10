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

template <typename BaseInteger, int msb, int lsb = msb>
class BitFieldMember
{
public:
    static const BaseInteger Msb = msb;
    static const BaseInteger Lsb = lsb;
    static const BaseInteger Width = Msb - Lsb + BaseInteger(1);
    static const BaseInteger Mask = ((BaseInteger(1) << Width) - BaseInteger(1)) << Lsb;
};

template <typename BaseInteger>
class BitField
{
public:
    template <int msb, int lsb = msb>
    using Member = BitFieldMember<BaseInteger, msb, lsb>;

    BitField()
    {
    }

    explicit BitField(BaseInteger value)
    {
        m_Value = value;
    }

    operator BaseInteger() const
    {
        return GetValue();
    }

    BaseInteger GetValue() const
    {
        return m_Value;
    }

    BitField& SetValue(BaseInteger value)
    {
        m_Value = value;
        return *this;
    }

    BaseInteger GetWithMask(BaseInteger mask) const
    {
        return (m_Value & mask);
    }

    BitField& SetWithMask(BaseInteger value, BaseInteger mask)
    {
        m_Value = (m_Value & ~mask) | (value & mask);
        return *this;
    }

    template <typename TMember>
    BaseInteger GetMember() const
    {
        return (m_Value & TMember::Mask) >> TMember::Lsb;
    }

    template <typename TMember>
    BitField& SetMember(BaseInteger value)
    {
        m_Value = (m_Value & ~TMember::Mask) | ((value << TMember::Lsb) & TMember::Mask);
        return *this;
    }

private:
    BaseInteger m_Value{};
};

using BitField32 = BitField<uint32_t>;
using BitField64 = BitField<uint64_t>;

}
