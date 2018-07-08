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

template <int msb, int lsb = msb>
class BitFieldMember
{
public:
    static const int Msb = msb;
    static const int Lsb = lsb;
    static const int Width = Msb - Lsb + 1;
    static const int Mask = ((1 << Width) - 1) << Lsb;
};

class BitField
{
public:
    explicit BitField(uint32_t value)
    {
        m_Value = value;
    }

    explicit BitField(int32_t value)
    {
        m_Value = static_cast<uint32_t>(value);
    }

    uint32_t GetWithMask(uint32_t mask) const
    {
        return (m_Value & mask);
    }

    uint32_t GetWithMask(int32_t mask) const
    {
        return GetWithMask(static_cast<uint32_t>(mask));
    }

    BitField& SetWithMask(uint32_t value, uint32_t mask)
    {
        m_Value = (m_Value & ~mask) | (value & mask);
        return *this;
    }

    BitField& SetWithMask(int32_t value, int32_t mask)
    {
        SetWithMask(static_cast<uint32_t>(value), static_cast<uint32_t>(mask));
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

    BitField& Set(int32_t value)
    {
        m_Value = value;
        return *this;
    }

    BitField& Set(uint32_t value)
    {
        m_Value = value;
        return *this;
    }

    int32_t GetInt32() const
    {
        return m_Value;
    }

    uint32_t GetUInt32() const
    {
        return m_Value;
    }

    operator int32_t() const
    {
        return GetInt32();
    }

    operator uint32_t() const
    {
        return GetUInt32();
    }
private:
    uint32_t m_Value;
};
