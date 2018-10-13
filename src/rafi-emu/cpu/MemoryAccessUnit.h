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
#include <cstring>

#include <rafi/BitField.h>

#include "../bus/Bus.h"

#include "Csr.h"

using namespace rvtrace;

namespace rafi { namespace cpu {

class PageTableEntry : public BitField
{
public:
    explicit PageTableEntry(int32_t value)
        : BitField(value)
    {
    }

    using Valid = BitFieldMember<0>;
    using Read = BitFieldMember<1>;
    using Write = BitFieldMember<2>;
    using Execute = BitFieldMember<3>;
    using User = BitFieldMember<4>;
    using Global = BitFieldMember<5>;
    using Accessed = BitFieldMember<6>;
    using Dirty = BitFieldMember<7>;

    using PhysicalPageNumber = BitFieldMember<31, 10>;
    using PhysicalPageNumber0 = BitFieldMember<19, 10>;
    using PhysicalPageNumber1 = BitFieldMember<31, 20>;
};

class MemoryAccessUnit
{
public:
    MemoryAccessUnit()
    {
        std::memset(&m_Event, 0, sizeof(m_Event));
    }

    void Initialize(bus::Bus* pBus, Csr* pCsr)
    {
        m_pBus = pBus;
        m_pCsr = pCsr;
    }

    int8_t LoadInt8(int32_t virtualAddress);
    void StoreInt8(int32_t virtualAddress, int8_t value);

    int16_t LoadInt16(int32_t virtualAddress);
    void StoreInt16(int32_t virtualAddress, int16_t value);

    int32_t FetchInt32(PhysicalAddress* outPhysicalAddress, int32_t virtualAddress);
    int32_t LoadInt32(int32_t virtualAddress);
    void StoreInt32(int32_t virtualAddress, int32_t value);

    std::optional<Trap> CheckTrap(MemoryAccessType accessType, int32_t pc, int32_t virtualAddress) const;

    // for Dump
    void ClearEvent();
    void CopyEvent(MemoryAccessEvent* pOut) const;
    bool IsEventExist() const;

private:
    const int PageTableEntrySize = 4;

    bool IsAddresssTranslationEnabled() const;
    bool IsLeafEntry(const PageTableEntry& entry) const;

    std::optional<Trap> CheckTrapForEntry(const PageTableEntry& entry, MemoryAccessType accessType, int32_t pc, int32_t virtualAddress) const;
    std::optional<Trap> CheckTrapForLeafEntry(const PageTableEntry& entry, MemoryAccessType accessType, int32_t pc, int32_t virtualAddress) const;

    std::optional<Trap> MakeTrap(MemoryAccessType accessType, int32_t pc, int32_t virtualAddress) const;

    PhysicalAddress ProcessTranslation(int32_t virtualAddress, bool isWrite);
    void UpdateEntry(PhysicalAddress entryAddress, bool isWrite);

    bus::Bus* m_pBus = nullptr;
    Csr* m_pCsr = nullptr;

    bool m_EventValid = false;

    MemoryAccessEvent m_Event;
};

}}
