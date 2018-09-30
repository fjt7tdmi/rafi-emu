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

#include "../../Common//Exception.h"
#include "MemoryAccessUnit.h"

using namespace rvtrace;

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
    auto physicalAddress = ProcessTranslation(virtualAddress, false);
    auto value = m_pBus->GetInt8(physicalAddress);

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
    auto physicalAddress = ProcessTranslation(virtualAddress, false);
    auto value = m_pBus->GetInt16(physicalAddress);

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
    auto physicalAddress = ProcessTranslation(virtualAddress, true);
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
    auto physicalAddress = ProcessTranslation(virtualAddress, false);
    auto value = m_pBus->GetInt32(physicalAddress);

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
    auto physicalAddress = ProcessTranslation(virtualAddress, true);
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

void MemoryAccessUnit::CheckException(MemoryAccessType accessType, int32_t pc, int32_t virtualAddress)
{
    // TODO: Implement Physical Memory Protection (PMP)

    if (!m_pCsr->IsAddresssTranslationEnabled())
    {
        return;
    }

    auto va = VirtualAddress(virtualAddress);

    auto firstTableHead = PageSize * m_pCsr->GetPhysicalPageNumber();
    auto firstEntryAddress = firstTableHead + PageTableEntrySize * va.GetMember<VirtualAddress::VirtualPageNumber1>();
    auto firstEntry = PageTableEntry(m_pBus->GetInt32(firstEntryAddress));

    CheckExceptionForEntry(firstEntry, accessType, pc, virtualAddress);

    if (IsLeafEntry(firstEntry))
    {
        CheckExceptionForLeafEntry(firstEntry, accessType, pc, virtualAddress);

        if (firstEntry.GetMember<PageTableEntry::PhysicalPageNumber0>() != 0)
        {
            RaiseException(accessType, pc, virtualAddress);
        }
    }
    else
    {
        auto secondTableHead = PageSize * firstEntry.GetMember<PageTableEntry::PhysicalPageNumber>();
        auto secondEntryAddress = secondTableHead + PageTableEntrySize * va.GetMember<VirtualAddress::VirtualPageNumber0>();
        auto secondEntry = PageTableEntry(m_pBus->GetInt32(secondEntryAddress));

        CheckExceptionForEntry(secondEntry, accessType, pc, virtualAddress);
        CheckExceptionForLeafEntry(secondEntry, accessType, pc, virtualAddress);
    }
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

bool MemoryAccessUnit::IsLeafEntry(const PageTableEntry& entry)
{
    return entry.GetMember<PageTableEntry::Read>() || entry.GetMember<PageTableEntry::Execute>();
}

void MemoryAccessUnit::CheckExceptionForEntry(const PageTableEntry& entry, MemoryAccessType accessType, int32_t pc, int32_t virtualAddress)
{
    if (!entry.GetMember<PageTableEntry::Valid>() || (!entry.GetMember<PageTableEntry::Read>() && entry.GetMember<PageTableEntry::Write>()))
    {
        RaiseException(accessType, pc, virtualAddress);
    }
}

void MemoryAccessUnit::CheckExceptionForLeafEntry(const PageTableEntry& entry, MemoryAccessType accessType, int32_t pc, int32_t virtualAddress)
{
    PrivilegeLevel privilegeLevel = m_pCsr->GetPrivilegeLevel();
    bool supervisorCanAccessUserMemory = m_pCsr->GetSupervisorUserMemory();
    bool makeExecutableReadable = m_pCsr->GetMakeExecutableReadable();

    switch (privilegeLevel)
    {
    case PrivilegeLevel::Supervisor:
        if (!supervisorCanAccessUserMemory && entry.GetMember<PageTableEntry::User>())
        {
            RaiseException(accessType, pc, virtualAddress);
        }
        break;
    case PrivilegeLevel::User:
        if (!entry.GetMember<PageTableEntry::User>())
        {
            RaiseException(accessType, pc, virtualAddress);
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
            RaiseException(accessType, pc, virtualAddress);
        }
        break;
    case MemoryAccessType::Load:
        if (!entry.GetMember<PageTableEntry::Read>() && !(makeExecutableReadable && entry.GetMember<PageTableEntry::Execute>()))
        {
            RaiseException(accessType, pc, virtualAddress);
        }
        break;
    case MemoryAccessType::Store:
        if (!entry.GetMember<PageTableEntry::Write>())
        {
            RaiseException(accessType, pc, virtualAddress);
        }
        break;
    default:
        throw NotImplementedException(__FILE__, __LINE__);
    }
}

void MemoryAccessUnit::RaiseException(MemoryAccessType accessType, int32_t pc, int32_t virtualAddress)
{
    switch (accessType)
    {
    case MemoryAccessType::Instruction:
        throw InstructionPageFaultException(pc);
    case MemoryAccessType::Load:
        throw LoadPageFaultException(pc, virtualAddress);
    case MemoryAccessType::Store:
        throw StorePageFaultException(pc, virtualAddress);
    default:
        throw NotImplementedException(__FILE__, __LINE__);
    }
}

PhysicalAddress MemoryAccessUnit::ProcessTranslation(int32_t virtualAddress, bool isWrite)
{
    if (m_pCsr->IsAddresssTranslationEnabled())
    {
        auto va = VirtualAddress(virtualAddress);

        PhysicalAddress firstTableHead = static_cast<uint64_t>(PageSize) * m_pCsr->GetPhysicalPageNumber();
        PhysicalAddress firstEntryAddress = firstTableHead + PageTableEntrySize * va.GetMember<VirtualAddress::VirtualPageNumber1>();
        auto firstEntry = PageTableEntry(m_pBus->GetInt32(firstEntryAddress));

        PhysicalAddress physicalAddress;

        if (IsLeafEntry(firstEntry))
        {
            UpdateEntry(firstEntryAddress, isWrite);

            physicalAddress = MegaPageSize * firstEntry.GetMember<PageTableEntry::PhysicalPageNumber1>() + va.GetMember<VirtualAddress::MegaOffset>();
        }
        else
        {
            PhysicalAddress secondTableHead = static_cast<uint64_t>(PageSize) * firstEntry.GetMember<PageTableEntry::PhysicalPageNumber>();
            PhysicalAddress secondEntryAddress = secondTableHead + PageTableEntrySize * va.GetMember<VirtualAddress::VirtualPageNumber0>();
            auto secondEntry = PageTableEntry(m_pBus->GetInt32(secondEntryAddress));

            UpdateEntry(secondEntryAddress, isWrite);

            physicalAddress = PageSize * secondEntry.GetMember<PageTableEntry::PhysicalPageNumber>() + va.GetMember<VirtualAddress::Offset>();
        }

        return physicalAddress;
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
