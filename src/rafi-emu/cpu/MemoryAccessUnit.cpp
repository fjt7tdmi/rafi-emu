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

#include <cstdint>
#include <fstream>

#include <rafi/emu.h>

#include "MemoryAccessUnit.h"

namespace rafi { namespace emu { namespace cpu {

namespace {
    const int PageSize = 1 << 12;
    const int MegaPageSize = 1 << 22;
    const int PageTableEntrySize = 4;

    class VirtualAddress : public BitField32
    {
    public:
        explicit VirtualAddress(uint32_t value)
            : BitField32(static_cast<uint32_t>(value))
        {
        }

        using Offset = Member<11, 0>;
        using MegaOffset = Member<21, 0>;
        using VirtualPageNumber0 = Member<21, 12>;
        using VirtualPageNumber1 = Member<31, 22>;
    };
}

uint8_t MemoryAccessUnit::LoadUInt8(uint32_t virtualAddress)
{
    const auto physicalAddress = ProcessTranslation(virtualAddress, false);
    const auto value = m_pBus->ReadUInt8(physicalAddress);

    AddEvent(MemoryAccessType::Load, sizeof(value), value, virtualAddress, physicalAddress);

    return value;
}

uint16_t MemoryAccessUnit::LoadUInt16(uint32_t virtualAddress)
{
    const auto physicalAddress = ProcessTranslation(virtualAddress, false);
    const auto value = m_pBus->ReadUInt16(physicalAddress);

    AddEvent(MemoryAccessType::Load, sizeof(value), value, virtualAddress, physicalAddress);

    return value;
}

uint32_t MemoryAccessUnit::LoadUInt32(uint32_t virtualAddress)
{
    const auto physicalAddress = ProcessTranslation(virtualAddress, false);
    const auto value = m_pBus->ReadUInt32(physicalAddress);

    AddEvent(MemoryAccessType::Load, sizeof(value), value, virtualAddress, physicalAddress);

    return value;
}

uint64_t MemoryAccessUnit::LoadUInt64(uint32_t virtualAddress)
{
    const auto physicalAddress = ProcessTranslation(virtualAddress, false);
    const auto value = m_pBus->ReadUInt64(physicalAddress);

    AddEvent(MemoryAccessType::Load, sizeof(value), value, virtualAddress, physicalAddress);

    return value;
}

void MemoryAccessUnit::StoreUInt8(uint32_t virtualAddress, uint8_t value)
{
    auto physicalAddress = ProcessTranslation(virtualAddress, true);
    m_pBus->WriteUInt8(physicalAddress, value);

    AddEvent(MemoryAccessType::Store, sizeof(value), value, virtualAddress, physicalAddress);
}

void MemoryAccessUnit::StoreUInt16(uint32_t virtualAddress, uint16_t value)
{
    const auto physicalAddress = ProcessTranslation(virtualAddress, true);
    m_pBus->WriteUInt16(physicalAddress, value);

    AddEvent(MemoryAccessType::Store, sizeof(value), value, virtualAddress, physicalAddress);
}

void MemoryAccessUnit::StoreUInt32(uint32_t virtualAddress, uint32_t value)
{
    const auto physicalAddress = ProcessTranslation(virtualAddress, true);
    m_pBus->WriteUInt32(physicalAddress, value);

    AddEvent(MemoryAccessType::Store, sizeof(value), value, virtualAddress, physicalAddress);
}

void MemoryAccessUnit::StoreUInt64(uint32_t virtualAddress, uint64_t value)
{
    const auto physicalAddress = ProcessTranslation(virtualAddress, true);
    m_pBus->WriteUInt64(physicalAddress, value);

    AddEvent(MemoryAccessType::Store, sizeof(value), value, virtualAddress, physicalAddress);
}

uint32_t MemoryAccessUnit::FetchUInt32(paddr_t* outPhysicalAddress, uint32_t virtualAddress)
{
    *outPhysicalAddress = ProcessTranslation(virtualAddress, false);
    return m_pBus->ReadUInt32(*outPhysicalAddress);
}

std::optional<Trap> MemoryAccessUnit::CheckTrap(MemoryAccessType accessType, uint32_t pc, uint32_t virtualAddress) const
{
    // TODO: Implement Physical Memory Protection (PMP)

    if (!IsAddresssTranslationEnabled())
    {
        return std::nullopt;
    }

    const auto va = VirtualAddress(virtualAddress);
    const auto satp = m_pCsr->ReadSatp();

    const auto firstTableHead = PageSize * satp.GetMember<satp_t::PPN>();
    const auto firstEntryAddress = firstTableHead + PageTableEntrySize * va.GetMember<VirtualAddress::VirtualPageNumber1>();
    const auto firstEntry = PageTableEntry(m_pBus->ReadUInt32(firstEntryAddress));

    const auto firstTrap = CheckTrapForEntry(firstEntry, accessType, pc, virtualAddress);
    if (firstTrap)
    {
        return firstTrap;
    }

    if (IsLeafEntry(firstEntry))
    {
        const auto leafTrap = CheckTrapForLeafEntry(firstEntry, accessType, pc, virtualAddress);
        if (leafTrap)
        {
            return leafTrap;
        }

        if (firstEntry.GetMember<PageTableEntry::PhysicalPageNumber0>() != 0)
        {
            return MakeTrap(accessType, pc, virtualAddress);
        }
    }
    else
    {
        const auto secondTableHead = PageSize * firstEntry.GetMember<PageTableEntry::PhysicalPageNumber>();
        const auto secondEntryAddress = secondTableHead + PageTableEntrySize * va.GetMember<VirtualAddress::VirtualPageNumber0>();
        const auto secondEntry = PageTableEntry(m_pBus->ReadUInt32(secondEntryAddress));

        const auto secondTrap = CheckTrapForEntry(secondEntry, accessType, pc, virtualAddress);
        if (secondTrap)
        {
            return secondTrap;
        }

        const auto leafTrap = CheckTrapForLeafEntry(secondEntry, accessType, pc, virtualAddress);
        if (leafTrap)
        {
            return leafTrap;
        }
    }

    return std::nullopt;
}

void MemoryAccessUnit::AddEvent(MemoryAccessType accessType, int size, uint64_t value, uint64_t vaddr, paddr_t paddr)
{
    m_Events.push_back({ accessType, static_cast<uint32_t>(size), value, vaddr, paddr });
}

void MemoryAccessUnit::ClearEvent()
{
    m_Events.clear();
}

void MemoryAccessUnit::CopyEvent(MemoryAccessEvent* pOut, int index) const
{
    *pOut = m_Events[index];
}

int MemoryAccessUnit::GetEventCount() const
{
    return m_Events.size();
}

bool MemoryAccessUnit::IsAddresssTranslationEnabled() const
{
    if (m_pCsr->GetPrivilegeLevel() == PrivilegeLevel::Machine)
    {
        return false;
    }

    const auto satp = m_pCsr->ReadSatp();
    const auto mode = static_cast<satp_t::Mode>(satp.GetMember<satp_t::MODE>());

    return mode != satp_t::Mode::Bare;
}

bool MemoryAccessUnit::IsLeafEntry(const PageTableEntry& entry) const
{
    return entry.GetMember<PageTableEntry::Read>() || entry.GetMember<PageTableEntry::Execute>();
}

std::optional<Trap> MemoryAccessUnit::CheckTrapForEntry(const PageTableEntry& entry, MemoryAccessType accessType, uint32_t pc, uint32_t virtualAddress) const
{
    if (!entry.GetMember<PageTableEntry::Valid>() || (!entry.GetMember<PageTableEntry::Read>() && entry.GetMember<PageTableEntry::Write>()))
    {
        return MakeTrap(accessType, pc, virtualAddress);
    }

    return std::nullopt;
}

std::optional<Trap> MemoryAccessUnit::CheckTrapForLeafEntry(const PageTableEntry& entry, MemoryAccessType accessType, uint32_t pc, uint32_t virtualAddress) const
{
    const auto privilegeLevel = m_pCsr->GetPrivilegeLevel();
    const auto status = m_pCsr->ReadStatus();

    bool supervisorCanAccessUserMemory = status.GetMember<xstatus_t::SUM>();
    bool makeExecutableReadable = status.GetMember<xstatus_t::MXR>();

    switch (privilegeLevel)
    {
    case PrivilegeLevel::Supervisor:
        if (!supervisorCanAccessUserMemory && entry.GetMember<PageTableEntry::User>())
        {
            return MakeTrap(accessType, pc, virtualAddress);
        }
        break;
    case PrivilegeLevel::User:
        if (!entry.GetMember<PageTableEntry::User>())
        {
            return MakeTrap(accessType, pc, virtualAddress);
        }
        break;
    default:
        break;
    }

    switch (accessType)
    {
    case MemoryAccessType::Instruction:
        if (!entry.GetMember<PageTableEntry::Execute>())
        {
            return MakeTrap(accessType, pc, virtualAddress);
        }
        break;
    case MemoryAccessType::Load:
        if (!entry.GetMember<PageTableEntry::Read>() && !(makeExecutableReadable && entry.GetMember<PageTableEntry::Execute>()))
        {
            return MakeTrap(accessType, pc, virtualAddress);
        }
        break;
    case MemoryAccessType::Store:
        if (!entry.GetMember<PageTableEntry::Write>())
        {
            return MakeTrap(accessType, pc, virtualAddress);
        }
        break;
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }

    return std::nullopt;
}

std::optional<Trap> MemoryAccessUnit::MakeTrap(MemoryAccessType accessType, uint32_t pc, uint32_t virtualAddress) const
{
    switch (accessType)
    {
    case MemoryAccessType::Instruction:
        return MakeInstructionPageFaultException(pc);
    case MemoryAccessType::Load:
        return MakeLoadPageFaultException(pc, virtualAddress);
    case MemoryAccessType::Store:
        return MakeStorePageFaultException(pc, virtualAddress);
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }
}

paddr_t MemoryAccessUnit::ProcessTranslation(uint32_t virtualAddress, bool isWrite)
{
    if (IsAddresssTranslationEnabled())
    {
        const auto va = VirtualAddress(virtualAddress);
        const auto satp = m_pCsr->ReadSatp();

        const paddr_t firstTableHead = static_cast<uint64_t>(PageSize) * satp.GetMember<satp_t::PPN>();
        const paddr_t firstEntryAddress = firstTableHead + PageTableEntrySize * va.GetMember<VirtualAddress::VirtualPageNumber1>();
        const auto firstEntry = PageTableEntry(m_pBus->ReadUInt32(firstEntryAddress));

        if (IsLeafEntry(firstEntry))
        {
            UpdateEntry(firstEntryAddress, isWrite);

            return MegaPageSize * firstEntry.GetMember<PageTableEntry::PhysicalPageNumber1>() + va.GetMember<VirtualAddress::MegaOffset>();
        }
        else
        {
            const paddr_t secondTableHead = static_cast<uint64_t>(PageSize) * firstEntry.GetMember<PageTableEntry::PhysicalPageNumber>();
            const paddr_t secondEntryAddress = secondTableHead + PageTableEntrySize * va.GetMember<VirtualAddress::VirtualPageNumber0>();
            const auto secondEntry = PageTableEntry(m_pBus->ReadUInt32(secondEntryAddress));

            UpdateEntry(secondEntryAddress, isWrite);

            return PageSize * secondEntry.GetMember<PageTableEntry::PhysicalPageNumber>() + va.GetMember<VirtualAddress::Offset>();
        }
    }
    else
    {
        return 0x00000000ffffffff & virtualAddress;
    }
}

void MemoryAccessUnit::UpdateEntry(paddr_t entryAddress, bool isWrite)
{
    auto entry = PageTableEntry(m_pBus->ReadUInt32(entryAddress));

    entry.SetMember<PageTableEntry::Accessed>(1);
    if (isWrite)
    {
        entry.SetMember<PageTableEntry::Dirty>(1);
    }
    m_pBus->WriteUInt32(entryAddress, entry.GetValue());
}

}}}
