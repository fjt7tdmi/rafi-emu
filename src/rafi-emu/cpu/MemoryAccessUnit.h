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
#include <vector>

#include <rafi/emu.h>

#include "../bus/Bus.h"

#include "Csr.h"

namespace rafi { namespace emu { namespace cpu {

class PageTableEntry : public BitField32
{
public:
    explicit PageTableEntry(uint32_t value)
        : BitField32(value)
    {
    }

    using Valid = Member<0>;
    using Read = Member<1>;
    using Write = Member<2>;
    using Execute = Member<3>;
    using User = Member<4>;
    using Global = Member<5>;
    using Accessed = Member<6>;
    using Dirty = Member<7>;

    using PhysicalPageNumber = Member<31, 10>;
    using PhysicalPageNumber0 = Member<19, 10>;
    using PhysicalPageNumber1 = Member<31, 20>;
};

class MemoryAccessUnit
{
public:
    MemoryAccessUnit()
    {
        m_Events.clear();
    }

    void Initialize(bus::Bus* pBus, Csr* pCsr)
    {
        m_pBus = pBus;
        m_pCsr = pCsr;
    }

    uint8_t LoadUInt8(uint32_t virtualAddress);
    uint16_t LoadUInt16(uint32_t virtualAddress);
    uint32_t LoadUInt32(uint32_t virtualAddress);
    uint64_t LoadUInt64(uint32_t virtualAddress);

    void StoreUInt8(uint32_t virtualAddress, uint8_t value);
    void StoreUInt16(uint32_t virtualAddress, uint16_t value);
    void StoreUInt32(uint32_t virtualAddress, uint32_t value);
    void StoreUInt64(uint32_t virtualAddress, uint64_t value);

    uint32_t FetchUInt32(PhysicalAddress* outPhysicalAddress, uint32_t virtualAddress);

    std::optional<Trap> CheckTrap(MemoryAccessType accessType, uint32_t pc, uint32_t virtualAddress) const;

    // for Dump
    void AddEvent(MemoryAccessType accessType, int size,  uint64_t value, uint64_t vaddr, PhysicalAddress paddr);
    void ClearEvent();

    void CopyEvent(MemoryAccessEvent* pOut, int index) const;
    int GetEventCount() const;

private:
    const int PageTableEntrySize = 4;

    bool IsAddresssTranslationEnabled() const;
    bool IsLeafEntry(const PageTableEntry& entry) const;

    std::optional<Trap> CheckTrapForEntry(const PageTableEntry& entry, MemoryAccessType accessType, uint32_t pc, uint32_t virtualAddress) const;
    std::optional<Trap> CheckTrapForLeafEntry(const PageTableEntry& entry, MemoryAccessType accessType, uint32_t pc, uint32_t virtualAddress) const;

    std::optional<Trap> MakeTrap(MemoryAccessType accessType, uint32_t pc, uint32_t virtualAddress) const;

    PhysicalAddress ProcessTranslation(uint32_t virtualAddress, bool isWrite);
    void UpdateEntry(PhysicalAddress entryAddress, bool isWrite);

    std::vector<MemoryAccessEvent> m_Events;

    bus::Bus* m_pBus{ nullptr };
    Csr* m_pCsr{ nullptr };
};

}}}
