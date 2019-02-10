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

template <int msb, int lsb = msb>
class BitFieldMember
{
public:
    static const uint32_t Msb = msb;
    static const uint32_t Lsb = lsb;
    static const uint32_t Width = Msb - Lsb + 1u;
    static const uint32_t Mask = ((1u << Width) - 1u) << Lsb;
};

class BitField
{
public:
    BitField()
    {
    }

    explicit BitField(uint32_t value)
    {
        m_Value = value;
    }

    operator uint32_t() const
    {
        return GetValue();
    }

    uint32_t GetValue() const
    {
        return m_Value;
    }

    BitField& SetValue(uint32_t value)
    {
        m_Value = value;
        return *this;
    }

    uint32_t GetWithMask(uint32_t mask) const
    {
        return (m_Value & mask);
    }

    BitField& SetWithMask(uint32_t value, uint32_t mask)
    {
        m_Value = (m_Value & ~mask) | (value & mask);
        return *this;
    }

    template <typename Member>
    uint32_t GetMember() const
    {
        return (m_Value & Member::Mask) >> Member::Lsb;
    }

    template <typename Member>
    BitField& SetMember(uint32_t value)
    {
        m_Value = (m_Value & ~Member::Mask) | ((value << Member::Lsb) & Member::Mask);
        return *this;
    }

private:
    uint32_t m_Value{};
};

}
