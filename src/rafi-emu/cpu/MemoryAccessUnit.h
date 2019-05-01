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

class MemoryAccessUnit
{
public:
    explicit MemoryAccessUnit(XLEN xlen);

    void Initialize(bus::Bus* pBus, Csr* pCsr);

    uint8_t LoadUInt8(vaddr_t addr);
    uint16_t LoadUInt16(vaddr_t addr);
    uint32_t LoadUInt32(vaddr_t addr);
    uint64_t LoadUInt64(vaddr_t addr);

    void StoreUInt8(vaddr_t addr, uint8_t value);
    void StoreUInt16(vaddr_t addr, uint16_t value);
    void StoreUInt32(vaddr_t addr, uint32_t value);
    void StoreUInt64(vaddr_t addr, uint64_t value);

    uint32_t FetchUInt32(paddr_t* outPhysicalAddress, vaddr_t addr);

    std::optional<Trap> CheckTrap(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const;

    // for Dump
    void AddEvent(MemoryAccessType accessType, int size,  vaddr_t value, vaddr_t vaddr, paddr_t paddr);
    void ClearEvent();

    void CopyEvent(MemoryAccessEvent* pOut, int index) const;
    int GetEventCount() const;

private:
    PrivilegeLevel GetEffectivePrivilegeLevel(MemoryAccessType accessType) const;

    AddressTranslationMode GetAddresssTranslationMode(MemoryAccessType accessType) const;

    std::optional<Trap> CheckTrapSv32(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const;
    std::optional<Trap> CheckTrapSv39(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const;
    std::optional<Trap> CheckTrapSv48(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const;
    std::optional<Trap> CheckTrapSv57(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const;
    std::optional<Trap> CheckTrapSv64(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const;

    std::optional<Trap> MakeTrap(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const;

    paddr_t Translate(MemoryAccessType accessType, vaddr_t addr);
    paddr_t TranslateSv32(vaddr_t addr, bool isWrite);
    paddr_t TranslateSv39(vaddr_t addr, bool isWrite);
    paddr_t TranslateSv48(vaddr_t addr, bool isWrite);
    paddr_t TranslateSv57(vaddr_t addr, bool isWrite);
    paddr_t TranslateSv64(vaddr_t addr, bool isWrite);

    template <typename EntryType>
    std::optional<Trap> CheckTrapForEntry(const EntryType& entry, MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
    {
        if (!entry.template GetMember<typename EntryType::V>() ||
            (!entry.template GetMember<typename EntryType::R>() && entry.template GetMember<typename EntryType::W>()))
        {
            return MakeTrap(accessType, pc, addr);
        }

        return std::nullopt;
    }

    template <typename EntryType>
    std::optional<Trap> CheckTrapForLeafEntry(const EntryType& entry, MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
    {
        const auto privilegeLevel = GetEffectivePrivilegeLevel(accessType);

        const auto status = m_pCsr->ReadStatus();
        const bool sum = status.GetMember<xstatus_t::SUM>();
        const bool mxr = status.GetMember<xstatus_t::MXR>();

        switch (privilegeLevel)
        {
        case PrivilegeLevel::Supervisor:
            if (!sum && entry.template GetMember<typename EntryType::U>())
            {
                return MakeTrap(accessType, pc, addr);
            }
            break;
        case PrivilegeLevel::User:
            if (!entry.template GetMember<typename EntryType::U>())
            {
                return MakeTrap(accessType, pc, addr);
            }
            break;
        default:
            break;
        }

        switch (accessType)
        {
        case MemoryAccessType::Instruction:
            if (!entry.template GetMember<typename EntryType::E>())
            {
                return MakeTrap(accessType, pc, addr);
            }
            break;
        case MemoryAccessType::Load:
            if (!entry.template GetMember<typename EntryType::R>() &&
                !(mxr && entry.template GetMember<typename EntryType::E>()))
            {
                return MakeTrap(accessType, pc, addr);
            }
            break;
        case MemoryAccessType::Store:
            if (!entry.template GetMember<typename EntryType::W>())
            {
                return MakeTrap(accessType, pc, addr);
            }
            break;
        default:
            RAFI_EMU_NOT_IMPLEMENTED();
        }

        return std::nullopt;
    }

    template <typename EntryType>
    void UpdateEntry(paddr_t entryAddress, bool isWrite)
    {
        static_assert(sizeof(EntryType) == 4 || sizeof(EntryType) == 8);

        EntryType entry(0);
        if constexpr (sizeof(EntryType) == 4)
        {
            entry = EntryType(m_pBus->ReadUInt32(entryAddress));
        }
        else
        {
            entry = EntryType(m_pBus->ReadUInt64(entryAddress));
        }

        entry.template SetMember<typename EntryType::A>(1);
        if (isWrite)
        {
            entry.template SetMember<typename EntryType::D>(1);
        }

        if constexpr (sizeof(EntryType) == 4)
        {
            m_pBus->WriteUInt32(entryAddress, entry.GetValue());
        }
        else
        {
            m_pBus->WriteUInt64(entryAddress, entry.GetValue());
        }
    }

    template <typename EntryType>
    bool IsLeafEntry(const EntryType& entry) const
    {
        return entry.template GetMember<typename EntryType::R>() || entry.template GetMember<typename EntryType::E>();
    }

    bus::Bus* m_pBus{ nullptr };
    Csr* m_pCsr{ nullptr };

    XLEN m_XLEN;

    std::vector<MemoryAccessEvent> m_Events;
};

}}}
