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

MemoryAccessUnit::MemoryAccessUnit(XLEN xlen)
    : m_XLEN(xlen)
{
    m_Events.clear();
}

void MemoryAccessUnit::Initialize(bus::Bus* pBus, Csr* pCsr)
{
    m_pBus = pBus;
    m_pCsr = pCsr;
}

uint8_t MemoryAccessUnit::LoadUInt8(vaddr_t addr)
{
    const auto physicalAddress = Translate(addr, false);
    const auto value = m_pBus->ReadUInt8(physicalAddress);

    AddEvent(MemoryAccessType::Load, sizeof(value), value, addr, physicalAddress);

    return value;
}

uint16_t MemoryAccessUnit::LoadUInt16(vaddr_t addr)
{
    const auto physicalAddress = Translate(addr, false);
    const auto value = m_pBus->ReadUInt16(physicalAddress);

    AddEvent(MemoryAccessType::Load, sizeof(value), value, addr, physicalAddress);

    return value;
}

uint32_t MemoryAccessUnit::LoadUInt32(vaddr_t addr)
{
    const auto physicalAddress = Translate(addr, false);
    const auto value = m_pBus->ReadUInt32(physicalAddress);

    AddEvent(MemoryAccessType::Load, sizeof(value), value, addr, physicalAddress);

    return value;
}

uint64_t MemoryAccessUnit::LoadUInt64(vaddr_t addr)
{
    const auto physicalAddress = Translate(addr, false);
    const auto value = m_pBus->ReadUInt64(physicalAddress);

    AddEvent(MemoryAccessType::Load, sizeof(value), value, addr, physicalAddress);

    return value;
}

void MemoryAccessUnit::StoreUInt8(vaddr_t addr, uint8_t value)
{
    auto physicalAddress = Translate(addr, true);
    m_pBus->WriteUInt8(physicalAddress, value);

    AddEvent(MemoryAccessType::Store, sizeof(value), value, addr, physicalAddress);
}

void MemoryAccessUnit::StoreUInt16(vaddr_t addr, uint16_t value)
{
    const auto physicalAddress = Translate(addr, true);
    m_pBus->WriteUInt16(physicalAddress, value);

    AddEvent(MemoryAccessType::Store, sizeof(value), value, addr, physicalAddress);
}

void MemoryAccessUnit::StoreUInt32(vaddr_t addr, uint32_t value)
{
    const auto physicalAddress = Translate(addr, true);
    m_pBus->WriteUInt32(physicalAddress, value);

    AddEvent(MemoryAccessType::Store, sizeof(value), value, addr, physicalAddress);
}

void MemoryAccessUnit::StoreUInt64(vaddr_t addr, uint64_t value)
{
    const auto physicalAddress = Translate(addr, true);
    m_pBus->WriteUInt64(physicalAddress, value);

    AddEvent(MemoryAccessType::Store, sizeof(value), value, addr, physicalAddress);
}

uint32_t MemoryAccessUnit::FetchUInt32(paddr_t* outPhysicalAddress, vaddr_t addr)
{
    *outPhysicalAddress = Translate(addr, false);
    return m_pBus->ReadUInt32(*outPhysicalAddress);
}

std::optional<Trap> MemoryAccessUnit::CheckTrap(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
{
    // TODO: Implement Physical Memory Protection (PMP)

    switch (GetAddresssTranslationMode())
    {
    case AddressTranslationMode::Bare:
        return std::nullopt;
    case AddressTranslationMode::Sv32:
        return CheckTrapSv32(accessType, pc, addr);
    case AddressTranslationMode::Sv39:
        return CheckTrapSv39(accessType, pc, addr);
    case AddressTranslationMode::Sv48:
        return CheckTrapSv48(accessType, pc, addr);
    case AddressTranslationMode::Sv57:
        return CheckTrapSv57(accessType, pc, addr);
    case AddressTranslationMode::Sv64:
        return CheckTrapSv64(accessType, pc, addr);
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }
}

void MemoryAccessUnit::AddEvent(MemoryAccessType accessType, int size, uint64_t value, vaddr_t vaddr, paddr_t paddr)
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

AddressTranslationMode MemoryAccessUnit::GetAddresssTranslationMode() const
{
    if (m_pCsr->GetPrivilegeLevel() == PrivilegeLevel::Machine)
    {
        return AddressTranslationMode::Bare;
    }

    const auto satp = m_pCsr->ReadSatp();

    switch (m_XLEN)
    {
    case XLEN::XLEN32:
        return static_cast<AddressTranslationMode>(satp.GetMember<satp_t::MODE_RV32>());
    case XLEN::XLEN64:
        return static_cast<AddressTranslationMode>(satp.GetMember<satp_t::MODE_RV64>());
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }
}

std::optional<Trap> MemoryAccessUnit::CheckTrapSv32(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
{
    const auto vaddr = VirtualAddressSv32(addr);
    const auto satp = m_pCsr->ReadSatp();

    PhysicalAddressSv32 entryAddr1(0);
    entryAddr1.SetMember<PhysicalAddressSv32::PPN>(satp.GetMember<satp_t::PPN_RV32>());
    entryAddr1.SetMember<PhysicalAddressSv32::Offset>(sizeof(PageTableEntrySv32) * vaddr.GetMember<VirtualAddressSv32::VPN1>());

    const auto entry1 = PageTableEntrySv32(m_pBus->ReadUInt32(entryAddr1));

    const auto trap1 = CheckTrapForEntry(entry1, accessType, pc, addr);
    if (trap1)
    {
        return trap1;
    }

    if (IsLeafEntry(entry1))
    {
        const auto trapLeaf = CheckTrapForLeafEntry(entry1, accessType, pc, addr);
        if (trapLeaf)
        {
            return trapLeaf;
        }

        if (entry1.GetMember<PageTableEntrySv32::PPN0>() != 0)
        {
            return MakeTrap(accessType, pc, addr);
        }

        return std::nullopt;
    }

    PhysicalAddressSv32 entryAddr2(0);
    entryAddr2.SetMember<PhysicalAddressSv32::PPN1>(entry1.GetMember<PageTableEntrySv32::PPN1>());
    entryAddr2.SetMember<PhysicalAddressSv32::PPN0>(entry1.GetMember<PageTableEntrySv32::PPN0>());
    entryAddr2.SetMember<PhysicalAddressSv32::Offset>(sizeof(PageTableEntrySv32) * vaddr.GetMember<VirtualAddressSv32::VPN0>());

    const auto entry2 = PageTableEntrySv32(m_pBus->ReadUInt32(entryAddr2));

    const auto trap2 = CheckTrapForEntry(entry2, accessType, pc, addr);
    if (trap2)
    {
        return trap2;
    }

    const auto trapLeaf = CheckTrapForLeafEntry(entry2, accessType, pc, addr);
    if (trapLeaf)
    {
        return trapLeaf;
    }

    return std::nullopt;
}

std::optional<Trap> MemoryAccessUnit::CheckTrapSv39(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
{
    (void)accessType;
    (void)pc;
    (void)addr;
    RAFI_EMU_NOT_IMPLEMENTED();
}

std::optional<Trap> MemoryAccessUnit::CheckTrapSv48(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
{
    (void)accessType;
    (void)pc;
    (void)addr;
    RAFI_EMU_NOT_IMPLEMENTED();
}

std::optional<Trap> MemoryAccessUnit::CheckTrapSv57(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
{
    (void)accessType;
    (void)pc;
    (void)addr;
    RAFI_EMU_NOT_IMPLEMENTED();
}

std::optional<Trap> MemoryAccessUnit::CheckTrapSv64(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
{
    (void)accessType;
    (void)pc;
    (void)addr;
    RAFI_EMU_NOT_IMPLEMENTED();
}

std::optional<Trap> MemoryAccessUnit::CheckTrapForEntry(const PageTableEntrySv32& entry, MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
{
    if (!entry.GetMember<PageTableEntrySv32::V>() || (!entry.GetMember<PageTableEntrySv32::R>() && entry.GetMember<PageTableEntrySv32::W>()))
    {
        return MakeTrap(accessType, pc, addr);
    }

    return std::nullopt;
}

std::optional<Trap> MemoryAccessUnit::CheckTrapForLeafEntry(const PageTableEntrySv32& entry, MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
{
    const auto privilegeLevel = m_pCsr->GetPrivilegeLevel();
    const auto status = m_pCsr->ReadStatus();

    bool supervisorCanAccessUserMemory = status.GetMember<xstatus_t::SUM>();
    bool makeExecutableReadable = status.GetMember<xstatus_t::MXR>();

    switch (privilegeLevel)
    {
    case PrivilegeLevel::Supervisor:
        if (!supervisorCanAccessUserMemory && entry.GetMember<PageTableEntrySv32::U>())
        {
            return MakeTrap(accessType, pc, addr);
        }
        break;
    case PrivilegeLevel::User:
        if (!entry.GetMember<PageTableEntrySv32::U>())
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
        if (!entry.GetMember<PageTableEntrySv32::E>())
        {
            return MakeTrap(accessType, pc, addr);
        }
        break;
    case MemoryAccessType::Load:
        if (!entry.GetMember<PageTableEntrySv32::R>() && !(makeExecutableReadable && entry.GetMember<PageTableEntrySv32::E>()))
        {
            return MakeTrap(accessType, pc, addr);
        }
        break;
    case MemoryAccessType::Store:
        if (!entry.GetMember<PageTableEntrySv32::W>())
        {
            return MakeTrap(accessType, pc, addr);
        }
        break;
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }

    return std::nullopt;
}

std::optional<Trap> MemoryAccessUnit::MakeTrap(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
{
    switch (accessType)
    {
    case MemoryAccessType::Instruction:
        return MakeInstructionPageFaultException(pc);
    case MemoryAccessType::Load:
        return MakeLoadPageFaultException(pc, addr);
    case MemoryAccessType::Store:
        return MakeStorePageFaultException(pc, addr);
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }
}

paddr_t MemoryAccessUnit::Translate(vaddr_t addr, bool isWrite)
{
    switch (GetAddresssTranslationMode())
    {
    case AddressTranslationMode::Bare:
        if (m_XLEN == XLEN::XLEN32)
        {
            return addr & 0x00000000ffffffff;
        }
        else
        {
            return static_cast<uint64_t>(addr);
        }
    case AddressTranslationMode::Sv32:
        return TranslateSv32(addr, isWrite);
    case AddressTranslationMode::Sv39:
        return TranslateSv39(addr, isWrite);
    case AddressTranslationMode::Sv48:
        return TranslateSv48(addr, isWrite);
    case AddressTranslationMode::Sv57:
        return TranslateSv57(addr, isWrite);
    case AddressTranslationMode::Sv64:
        return TranslateSv64(addr, isWrite);
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }
}

paddr_t MemoryAccessUnit::TranslateSv32(vaddr_t addr, bool isWrite)
{
    const auto vaddr = VirtualAddressSv32(addr);
    const auto satp = m_pCsr->ReadSatp();

    PhysicalAddressSv32 entryAddr1(0);
    entryAddr1.SetMember<PhysicalAddressSv32::PPN>(satp.GetMember<satp_t::PPN_RV32>());
    entryAddr1.SetMember<PhysicalAddressSv32::Offset>(sizeof(PageTableEntrySv32) * vaddr.GetMember<VirtualAddressSv32::VPN1>());

    const auto entry1 = PageTableEntrySv32(m_pBus->ReadUInt32(entryAddr1));

    if (IsLeafEntry(entry1))
    {
        UpdateEntry(entryAddr1, isWrite);

        PhysicalAddressSv32 paddr(0);
        paddr.SetMember<PhysicalAddressSv32::PPN1>(entry1.GetMember<PageTableEntrySv32::PPN1>());
        paddr.SetMember<PhysicalAddressSv32::PPN0>(vaddr.GetMember<VirtualAddressSv32::VPN0>());
        paddr.SetMember<PhysicalAddressSv32::Offset>(vaddr.GetMember<VirtualAddressSv32::Offset>());

        return paddr.GetValue() & 0x00000000ffffffff;
    }

    PhysicalAddressSv32 entryAddr2(0);
    entryAddr2.SetMember<PhysicalAddressSv32::PPN1>(entry1.GetMember<PageTableEntrySv32::PPN1>());
    entryAddr2.SetMember<PhysicalAddressSv32::PPN0>(entry1.GetMember<PageTableEntrySv32::PPN0>());
    entryAddr2.SetMember<PhysicalAddressSv32::Offset>(sizeof(PageTableEntrySv32) * vaddr.GetMember<VirtualAddressSv32::VPN0>());

    const auto entry2 = PageTableEntrySv32(m_pBus->ReadUInt32(entryAddr2));

    UpdateEntry(entryAddr2, isWrite);

    PhysicalAddressSv32 paddr(0);
    paddr.SetMember<PhysicalAddressSv32::PPN1>(entry2.GetMember<PageTableEntrySv32::PPN1>());
    paddr.SetMember<PhysicalAddressSv32::PPN0>(entry2.GetMember<PageTableEntrySv32::PPN0>());
    paddr.SetMember<PhysicalAddressSv32::Offset>(vaddr.GetMember<VirtualAddressSv32::Offset>());

    return paddr.GetValue() & 0x00000000ffffffff;

}

paddr_t MemoryAccessUnit::TranslateSv39(vaddr_t addr, bool isWrite)
{
    (void)addr;
    (void)isWrite;
    RAFI_EMU_NOT_IMPLEMENTED();
}

paddr_t MemoryAccessUnit::TranslateSv48(vaddr_t addr, bool isWrite)
{
    (void)addr;
    (void)isWrite;
    RAFI_EMU_NOT_IMPLEMENTED();
}

paddr_t MemoryAccessUnit::TranslateSv57(vaddr_t addr, bool isWrite)
{
    (void)addr;
    (void)isWrite;
    RAFI_EMU_NOT_IMPLEMENTED();
}

paddr_t MemoryAccessUnit::TranslateSv64(vaddr_t addr, bool isWrite)
{
    (void)addr;
    (void)isWrite;
    RAFI_EMU_NOT_IMPLEMENTED();
}

void MemoryAccessUnit::UpdateEntry(paddr_t entryAddress, bool isWrite)
{
    auto entry = PageTableEntrySv32(m_pBus->ReadUInt32(entryAddress));

    entry.SetMember<PageTableEntrySv32::A>(1);
    if (isWrite)
    {
        entry.SetMember<PageTableEntrySv32::D>(1);
    }
    m_pBus->WriteUInt32(entryAddress, entry.GetValue());
}

bool MemoryAccessUnit::IsLeafEntry(const PageTableEntrySv32& entry) const
{
    return entry.GetMember<PageTableEntrySv32::R>() || entry.GetMember<PageTableEntrySv32::E>();
}

}}}
