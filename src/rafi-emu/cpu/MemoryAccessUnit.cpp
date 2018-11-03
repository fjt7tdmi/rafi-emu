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

#include <rafi/Common.h>

#include "MemoryAccessUnit.h"

using namespace rvtrace;

namespace rafi { namespace emu { namespace cpu {

namespace {
    const int PageSize = 1 << 12;
    const int MegaPageSize = 1 << 22;
    const int PageTableEntrySize = 4;

    class VirtualAddress : public BitField
    {
    public:
        explicit VirtualAddress(int32_t value)
            : BitField(static_cast<uint32_t>(value))
        {
        }

        using Offset = BitFieldMember<11, 0>;
        using MegaOffset = BitFieldMember<21, 0>;
        using VirtualPageNumber0 = BitFieldMember<21, 12>;
        using VirtualPageNumber1 = BitFieldMember<31, 22>;
    };
}

int8_t MemoryAccessUnit::LoadInt8(int32_t virtualAddress)
{
    const auto physicalAddress = ProcessTranslation(virtualAddress, false);
    const auto value = m_pBus->GetInt8(physicalAddress);

    m_Event.accessType = MemoryAccessType::Load;
    m_Event.accessSize = MemoryAccessSize::Byte;
    m_Event.virtualAddress = virtualAddress;
    m_Event.physicalAddress = physicalAddress;
    m_Event.value = value;

    m_EventValid = true;

    return value;
}

void MemoryAccessUnit::StoreInt8(int32_t virtualAddress, int8_t value)
{
    auto physicalAddress = ProcessTranslation(virtualAddress, true);
    m_pBus->SetInt8(physicalAddress, value);

    m_Event.accessType = MemoryAccessType::Store;
    m_Event.accessSize = MemoryAccessSize::Byte;
    m_Event.virtualAddress = virtualAddress;
    m_Event.physicalAddress = physicalAddress;
    m_Event.value = value;

    m_EventValid = true;
}

int16_t MemoryAccessUnit::LoadInt16(int32_t virtualAddress)
{
    const auto physicalAddress = ProcessTranslation(virtualAddress, false);
    const auto value = m_pBus->GetInt16(physicalAddress);

    m_Event.accessType = MemoryAccessType::Load;
    m_Event.accessSize = MemoryAccessSize::HalfWord;
    m_Event.virtualAddress = virtualAddress;
    m_Event.physicalAddress = physicalAddress;
    m_Event.value = value;

    m_EventValid = true;

    return value;
}

void MemoryAccessUnit::StoreInt16(int32_t virtualAddress, int16_t value)
{
    const auto physicalAddress = ProcessTranslation(virtualAddress, true);
    m_pBus->SetInt16(physicalAddress, value);

    m_Event.accessType = MemoryAccessType::Store;
    m_Event.accessSize = MemoryAccessSize::HalfWord;
    m_Event.virtualAddress = virtualAddress;
    m_Event.physicalAddress = physicalAddress;
    m_Event.value = value;

    m_EventValid = true;
}

int32_t MemoryAccessUnit::LoadInt32(int32_t virtualAddress)
{
    const auto physicalAddress = ProcessTranslation(virtualAddress, false);
    const auto value = m_pBus->GetInt32(physicalAddress);

    m_Event.accessType = MemoryAccessType::Load;
    m_Event.accessSize = MemoryAccessSize::Word;
    m_Event.virtualAddress = virtualAddress;
    m_Event.physicalAddress = physicalAddress;
    m_Event.value = value;

    m_EventValid = true;

    return value;
}

void MemoryAccessUnit::StoreInt32(int32_t virtualAddress, int32_t value)
{
    const auto physicalAddress = ProcessTranslation(virtualAddress, true);
    m_pBus->SetInt32(physicalAddress, value);

    m_Event.accessType = MemoryAccessType::Store;
    m_Event.accessSize = MemoryAccessSize::Word;
    m_Event.virtualAddress = virtualAddress;
    m_Event.physicalAddress = physicalAddress;
    m_Event.value = value;

    m_EventValid = true;
}

int32_t MemoryAccessUnit::FetchInt32(PhysicalAddress* outPhysicalAddress, int32_t virtualAddress)
{
    *outPhysicalAddress = ProcessTranslation(virtualAddress, false);
    return m_pBus->GetInt32(*outPhysicalAddress);
}

std::optional<Trap> MemoryAccessUnit::CheckTrap(MemoryAccessType accessType, int32_t pc, int32_t virtualAddress) const
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
    const auto firstEntry = PageTableEntry(m_pBus->GetInt32(firstEntryAddress));

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
        const auto secondEntry = PageTableEntry(m_pBus->GetInt32(secondEntryAddress));

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

void MemoryAccessUnit::ClearEvent()
{
    m_EventValid = false;
}

void MemoryAccessUnit::CopyEvent(MemoryAccessEvent* pOut) const
{
    std::memcpy(pOut, &m_Event, sizeof(*pOut));
}

bool MemoryAccessUnit::IsEventExist() const
{
    return m_EventValid;
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

std::optional<Trap> MemoryAccessUnit::CheckTrapForEntry(const PageTableEntry& entry, MemoryAccessType accessType, int32_t pc, int32_t virtualAddress) const
{
    if (!entry.GetMember<PageTableEntry::Valid>() || (!entry.GetMember<PageTableEntry::Read>() && entry.GetMember<PageTableEntry::Write>()))
    {
        return MakeTrap(accessType, pc, virtualAddress);
    }

    return std::nullopt;
}

std::optional<Trap> MemoryAccessUnit::CheckTrapForLeafEntry(const PageTableEntry& entry, MemoryAccessType accessType, int32_t pc, int32_t virtualAddress) const
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
        ABORT();
    }

    return std::nullopt;
}

std::optional<Trap> MemoryAccessUnit::MakeTrap(MemoryAccessType accessType, int32_t pc, int32_t virtualAddress) const
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
        ABORT();
    }
}

PhysicalAddress MemoryAccessUnit::ProcessTranslation(int32_t virtualAddress, bool isWrite)
{
    if (IsAddresssTranslationEnabled())
    {
        const auto va = VirtualAddress(virtualAddress);
        const auto satp = m_pCsr->ReadSatp();

        const PhysicalAddress firstTableHead = static_cast<uint64_t>(PageSize) * satp.GetMember<satp_t::PPN>();
        const PhysicalAddress firstEntryAddress = firstTableHead + PageTableEntrySize * va.GetMember<VirtualAddress::VirtualPageNumber1>();
        const auto firstEntry = PageTableEntry(m_pBus->GetInt32(firstEntryAddress));

        if (IsLeafEntry(firstEntry))
        {
            UpdateEntry(firstEntryAddress, isWrite);

            return MegaPageSize * firstEntry.GetMember<PageTableEntry::PhysicalPageNumber1>() + va.GetMember<VirtualAddress::MegaOffset>();
        }
        else
        {
            const PhysicalAddress secondTableHead = static_cast<uint64_t>(PageSize) * firstEntry.GetMember<PageTableEntry::PhysicalPageNumber>();
            const PhysicalAddress secondEntryAddress = secondTableHead + PageTableEntrySize * va.GetMember<VirtualAddress::VirtualPageNumber0>();
            const auto secondEntry = PageTableEntry(m_pBus->GetInt32(secondEntryAddress));

            UpdateEntry(secondEntryAddress, isWrite);

            return PageSize * secondEntry.GetMember<PageTableEntry::PhysicalPageNumber>() + va.GetMember<VirtualAddress::Offset>();
        }
    }
    else
    {
        return 0x00000000ffffffff & virtualAddress;
    }
}

void MemoryAccessUnit::UpdateEntry(PhysicalAddress entryAddress, bool isWrite)
{
    auto entry = PageTableEntry(m_pBus->GetInt32(entryAddress));

    entry.SetMember<PageTableEntry::Accessed>(1);
    if (isWrite)
    {
        entry.SetMember<PageTableEntry::Dirty>(1);
    }
    m_pBus->SetInt32(entryAddress, entry.GetInt32());
}

}}}
