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
    paddr_t paddr;
    Translate(&paddr, MemoryAccessType::Load, addr);
    const auto value = m_pBus->ReadUInt8(paddr);

    AddEvent(MemoryAccessType::Load, sizeof(value), value, addr, paddr);

    return value;
}

uint16_t MemoryAccessUnit::LoadUInt16(vaddr_t addr)
{
    paddr_t paddr;
    Translate(&paddr, MemoryAccessType::Load, addr);
    const auto value = m_pBus->ReadUInt16(paddr);

    AddEvent(MemoryAccessType::Load, sizeof(value), value, addr, paddr);

    return value;
}

uint32_t MemoryAccessUnit::LoadUInt32(vaddr_t addr)
{
    paddr_t paddr;
    Translate(&paddr, MemoryAccessType::Load, addr);
    const auto value = m_pBus->ReadUInt32(paddr);

    AddEvent(MemoryAccessType::Load, sizeof(value), value, addr, paddr);

    return value;
}

uint64_t MemoryAccessUnit::LoadUInt64(vaddr_t addr)
{
    paddr_t paddr;
    Translate(&paddr, MemoryAccessType::Load, addr);
    const auto value = m_pBus->ReadUInt64(paddr);

    AddEvent(MemoryAccessType::Load, sizeof(value), value, addr, paddr);

    return value;
}

void MemoryAccessUnit::StoreUInt8(vaddr_t addr, uint8_t value)
{
    paddr_t paddr;
    Translate(&paddr, MemoryAccessType::Store, addr);
    m_pBus->WriteUInt8(paddr, value);

    AddEvent(MemoryAccessType::Store, sizeof(value), value, addr, paddr);
}

void MemoryAccessUnit::StoreUInt16(vaddr_t addr, uint16_t value)
{
    paddr_t paddr;
    Translate(&paddr, MemoryAccessType::Store, addr);
    m_pBus->WriteUInt16(paddr, value);

    AddEvent(MemoryAccessType::Store, sizeof(value), value, addr, paddr);
}

void MemoryAccessUnit::StoreUInt32(vaddr_t addr, uint32_t value)
{
    paddr_t paddr;
    Translate(&paddr, MemoryAccessType::Store, addr);
    m_pBus->WriteUInt32(paddr, value);

    AddEvent(MemoryAccessType::Store, sizeof(value), value, addr, paddr);
}

void MemoryAccessUnit::StoreUInt64(vaddr_t addr, uint64_t value)
{
    paddr_t paddr;
    Translate(&paddr, MemoryAccessType::Store, addr);
    m_pBus->WriteUInt64(paddr, value);

    AddEvent(MemoryAccessType::Store, sizeof(value), value, addr, paddr);
}

uint16_t MemoryAccessUnit::FetchUInt16(vaddr_t vaddr, paddr_t paddr)
{
    const auto value = m_pBus->ReadUInt16(paddr);

    AddEvent(MemoryAccessType::Instruction, sizeof(value), value, vaddr, paddr);

    return value;
}

uint32_t MemoryAccessUnit::FetchUInt32(vaddr_t vaddr, paddr_t paddr)
{
    const auto value = m_pBus->ReadUInt32(paddr);

    AddEvent(MemoryAccessType::Instruction, sizeof(value), value, vaddr, paddr);

    return value;
}

std::optional<Trap> MemoryAccessUnit::CheckTrap(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
{
    // TODO: Implement Physical Memory Protection (PMP)

    switch (GetAddresssTranslationMode(accessType))
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

size_t MemoryAccessUnit::GetEventCount() const
{
    return m_Events.size();
}

PrivilegeLevel MemoryAccessUnit::GetEffectivePrivilegeLevel(MemoryAccessType accessType) const
{
    const auto status = m_pCsr->ReadStatus();
    const bool mprv = status.GetMember<xstatus_t::MPRV>();

    if (mprv && accessType != MemoryAccessType::Instruction)
    {
        return static_cast<PrivilegeLevel>(status.GetMember<xstatus_t::MPP>());
    }
    else
    {
        return m_pCsr->GetPrivilegeLevel();
    }
}

AddressTranslationMode MemoryAccessUnit::GetAddresssTranslationMode(MemoryAccessType accessType) const
{
    if (GetEffectivePrivilegeLevel(accessType) == PrivilegeLevel::Machine)
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

    PhysicalAddressSv32 entryAddr1(
        satp.GetMember<satp_t::PPN_RV32>(),
        sizeof(PageTableEntrySv32) * vaddr.GetMember<VirtualAddressSv32::VPN1>());
    const auto entry1 = PageTableEntrySv32(m_pBus->ReadUInt32(entryAddr1));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry1, accessType, pc, addr));
    if (IsLeafEntry(entry1))
    {
        RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry1, accessType, pc, addr));

        if (entry1.GetMember<PageTableEntrySv32::PPN0>() != 0)
        {
            return MakeTrap(accessType, pc, addr);
        }

        return std::nullopt;
    }

    PhysicalAddressSv32 entryAddr2(
        entry1.GetMember<PageTableEntrySv32::PPN1>(),
        entry1.GetMember<PageTableEntrySv32::PPN0>(),
        sizeof(PageTableEntrySv32) * vaddr.GetMember<VirtualAddressSv32::VPN0>());
    const auto entry2 = PageTableEntrySv32(m_pBus->ReadUInt32(entryAddr2));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry2, accessType, pc, addr));
    return CheckTrapForLeafEntry(entry2, accessType, pc, addr);
}

std::optional<Trap> MemoryAccessUnit::CheckTrapSv39(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
{
    const auto vaddr = VirtualAddressSv39(addr);
    const auto satp = m_pCsr->ReadSatp();

    PhysicalAddressSv39 entryAddr1(
        satp.GetMember<satp_t::PPN_RV64>(),
        sizeof(PageTableEntrySv39) * vaddr.GetMember<VirtualAddressSv39::VPN2>());
    const auto entry1 = PageTableEntrySv39(m_pBus->ReadUInt64(entryAddr1));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry1, accessType, pc, addr));
    if (IsLeafEntry(entry1))
    {
        RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry1, accessType, pc, addr));

        if (entry1.GetMember<PageTableEntrySv39::PPN1>() != 0 ||
            entry1.GetMember<PageTableEntrySv39::PPN0>() != 0)
        {
            return MakeTrap(accessType, pc, addr);
        }

        return std::nullopt;
    }

    PhysicalAddressSv39 entryAddr2(
        entry1.GetMember<PageTableEntrySv39::PPN2>(),
        entry1.GetMember<PageTableEntrySv39::PPN1>(),
        entry1.GetMember<PageTableEntrySv39::PPN0>(),
        sizeof(PageTableEntrySv39) * vaddr.GetMember<VirtualAddressSv39::VPN1>());
    const auto entry2 = PageTableEntrySv39(m_pBus->ReadUInt64(entryAddr2));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry2, accessType, pc, addr));
    if (IsLeafEntry(entry2))
    {
        RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry2, accessType, pc, addr));

        if (entry2.GetMember<PageTableEntrySv39::PPN0>() != 0)
        {
            return MakeTrap(accessType, pc, addr);
        }

        return std::nullopt;
    }

    PhysicalAddressSv39 entryAddr3(
        entry2.GetMember<PageTableEntrySv39::PPN2>(),
        entry2.GetMember<PageTableEntrySv39::PPN1>(),
        entry2.GetMember<PageTableEntrySv39::PPN0>(),
        sizeof(PageTableEntrySv39) * vaddr.GetMember<VirtualAddressSv39::VPN0>());
    const auto entry3 = PageTableEntrySv39(m_pBus->ReadUInt64(entryAddr3));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry3, accessType, pc, addr));
    return CheckTrapForLeafEntry(entry3, accessType, pc, addr);
}

std::optional<Trap> MemoryAccessUnit::CheckTrapSv48(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
{
    const auto vaddr = VirtualAddressSv48(addr);
    const auto satp = m_pCsr->ReadSatp();

    PhysicalAddressSv48 entryAddr1(
        satp.GetMember<satp_t::PPN_RV64>(),
        sizeof(PageTableEntrySv48) * vaddr.GetMember<VirtualAddressSv48::VPN3>());
    const auto entry1 = PageTableEntrySv48(m_pBus->ReadUInt64(entryAddr1));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry1, accessType, pc, addr));
    if (IsLeafEntry(entry1))
    {
        RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry1, accessType, pc, addr));

        if (entry1.GetMember<PageTableEntrySv48::PPN2>() != 0 ||
            entry1.GetMember<PageTableEntrySv48::PPN1>() != 0 ||
            entry1.GetMember<PageTableEntrySv48::PPN0>() != 0)
        {
            return MakeTrap(accessType, pc, addr);
        }

        return std::nullopt;
    }

    PhysicalAddressSv48 entryAddr2(
        entry1.GetMember<PageTableEntrySv48::PPN3>(),
        entry1.GetMember<PageTableEntrySv48::PPN2>(),
        entry1.GetMember<PageTableEntrySv48::PPN1>(),
        entry1.GetMember<PageTableEntrySv48::PPN0>(),
        sizeof(PageTableEntrySv48) * vaddr.GetMember<VirtualAddressSv48::VPN2>());
    const auto entry2 = PageTableEntrySv48(m_pBus->ReadUInt64(entryAddr2));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry2, accessType, pc, addr));
    if (IsLeafEntry(entry2))
    {
        RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry2, accessType, pc, addr));

        if (entry2.GetMember<PageTableEntrySv48::PPN1>() != 0 ||
            entry2.GetMember<PageTableEntrySv48::PPN0>() != 0)
        {
            return MakeTrap(accessType, pc, addr);
        }

        return std::nullopt;
    }

    PhysicalAddressSv48 entryAddr3(
        entry2.GetMember<PageTableEntrySv48::PPN3>(),
        entry2.GetMember<PageTableEntrySv48::PPN2>(),
        entry2.GetMember<PageTableEntrySv48::PPN1>(),
        entry2.GetMember<PageTableEntrySv48::PPN0>(),
        sizeof(PageTableEntrySv48) * vaddr.GetMember<VirtualAddressSv48::VPN1>());
    const auto entry3 = PageTableEntrySv48(m_pBus->ReadUInt64(entryAddr3));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry3, accessType, pc, addr));
    if (IsLeafEntry(entry3))
    {
        RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry3, accessType, pc, addr));

        if (entry3.GetMember<PageTableEntrySv48::PPN0>() != 0)
        {
            return MakeTrap(accessType, pc, addr);
        }

        return std::nullopt;
    }

    PhysicalAddressSv48 entryAddr4(
        entry3.GetMember<PageTableEntrySv48::PPN3>(),
        entry3.GetMember<PageTableEntrySv48::PPN2>(),
        entry3.GetMember<PageTableEntrySv48::PPN1>(),
        entry3.GetMember<PageTableEntrySv48::PPN0>(),
        sizeof(PageTableEntrySv48) * vaddr.GetMember<VirtualAddressSv48::VPN1>());
    const auto entry4 = PageTableEntrySv48(m_pBus->ReadUInt64(entryAddr4));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry4, accessType, pc, addr));
    return CheckTrapForLeafEntry(entry4, accessType, pc, addr);
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

std::optional<Trap> MemoryAccessUnit::MakeTrap(MemoryAccessType accessType, vaddr_t pc, vaddr_t addr) const
{
    switch (accessType)
    {
    case MemoryAccessType::Instruction:
        return MakeInstructionPageFaultException(pc, addr);
    case MemoryAccessType::Load:
        return MakeLoadPageFaultException(pc, addr);
    case MemoryAccessType::Store:
        return MakeStorePageFaultException(pc, addr);
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }
}

std::optional<Trap> MemoryAccessUnit::Translate(paddr_t* pOutAddr, MemoryAccessType accessType, vaddr_t addr, vaddr_t pc)
{
    switch (GetAddresssTranslationMode(accessType))
    {
    case AddressTranslationMode::Bare:
        *pOutAddr = (m_XLEN == XLEN::XLEN32) ? ZeroExtend(32, addr) : static_cast<paddr_t>(addr);
        return std::nullopt;
    case AddressTranslationMode::Sv32:
        return TranslateSv32(pOutAddr, accessType, addr, pc);
    case AddressTranslationMode::Sv39:
        return TranslateSv39(pOutAddr, accessType, addr, pc);
    case AddressTranslationMode::Sv48:
        return TranslateSv48(pOutAddr, accessType, addr, pc);
    case AddressTranslationMode::Sv57:
        return TranslateSv57(pOutAddr, accessType, addr, pc);
    case AddressTranslationMode::Sv64:
        return TranslateSv64(pOutAddr, accessType, addr, pc);
    default:
        RAFI_EMU_NOT_IMPLEMENTED();
    }
}

std::optional<Trap> MemoryAccessUnit::TranslateSv32(paddr_t* pOutAddr, MemoryAccessType accessType, vaddr_t addr, vaddr_t pc)
{
    const bool isWrite = (accessType == (MemoryAccessType::Store));

    const auto vaddr = VirtualAddressSv32(addr);
    const auto satp = m_pCsr->ReadSatp();

    PhysicalAddressSv32 entryAddr1(
        satp.GetMember<satp_t::PPN_RV32>(),
        sizeof(PageTableEntrySv32) * vaddr.GetMember<VirtualAddressSv32::VPN1>());
    const auto entry1 = PageTableEntrySv32(m_pBus->ReadUInt32(entryAddr1));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry1, accessType, pc, addr));
    if (IsLeafEntry(entry1))
    {
        RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry1, accessType, pc, addr));
        UpdateEntry<PageTableEntrySv32>(entryAddr1, isWrite);

        PhysicalAddressSv32 paddr(
            entry1.GetMember<PageTableEntrySv32::PPN1>(),
            vaddr.GetMember<VirtualAddressSv32::VPN0>(),
            vaddr.GetMember<VirtualAddressSv32::Offset>());

        *pOutAddr = ZeroExtend(32, paddr.GetValue());
        return std::nullopt;
    }

    PhysicalAddressSv32 entryAddr2(
        entry1.GetMember<PageTableEntrySv32::PPN1>(),
        entry1.GetMember<PageTableEntrySv32::PPN0>(),
        sizeof(PageTableEntrySv32) * vaddr.GetMember<VirtualAddressSv32::VPN0>());
    const auto entry2 = PageTableEntrySv32(m_pBus->ReadUInt32(entryAddr2));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry2, accessType, pc, addr));
    RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry2, accessType, pc, addr));
    UpdateEntry<PageTableEntrySv32>(entryAddr2, isWrite);

    PhysicalAddressSv32 paddr(
        entry2.GetMember<PageTableEntrySv32::PPN1>(),
        entry2.GetMember<PageTableEntrySv32::PPN0>(),
        vaddr.GetMember<VirtualAddressSv32::Offset>());

    *pOutAddr = ZeroExtend(32, paddr.GetValue());
    return std::nullopt;
}

std::optional<Trap> MemoryAccessUnit::TranslateSv39(paddr_t* pOutAddr, MemoryAccessType accessType, vaddr_t addr, vaddr_t pc)
{
    const bool isWrite = (accessType == (MemoryAccessType::Store));

    const auto vaddr = VirtualAddressSv39(addr);
    const auto satp = m_pCsr->ReadSatp();

    PhysicalAddressSv39 entryAddr1(
        satp.GetMember<satp_t::PPN_RV64>(),
        sizeof(PageTableEntrySv39) * vaddr.GetMember<VirtualAddressSv39::VPN2>());
    const auto entry1 = PageTableEntrySv39(m_pBus->ReadUInt64(entryAddr1));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry1, accessType, pc, addr));
    if (IsLeafEntry(entry1))
    {
        RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry1, accessType, pc, addr));
        UpdateEntry<PageTableEntrySv39>(entryAddr1, isWrite);

        PhysicalAddressSv39 paddr(
            entry1.GetMember<PageTableEntrySv39::PPN2>(),
            vaddr.GetMember<VirtualAddressSv39::VPN1>(),
            vaddr.GetMember<VirtualAddressSv39::VPN0>(),
            vaddr.GetMember<VirtualAddressSv39::Offset>());
        *pOutAddr = ZeroExtend(39, paddr.GetValue());
        return std::nullopt;
    }

    PhysicalAddressSv39 entryAddr2(
        entry1.GetMember<PageTableEntrySv39::PPN2>(),
        entry1.GetMember<PageTableEntrySv39::PPN1>(),
        entry1.GetMember<PageTableEntrySv39::PPN0>(),
        sizeof(PageTableEntrySv39) * vaddr.GetMember<VirtualAddressSv39::VPN1>());
    const auto entry2 = PageTableEntrySv39(m_pBus->ReadUInt64(entryAddr2));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry2, accessType, pc, addr));
    if (IsLeafEntry(entry2))
    {
        RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry2, accessType, pc, addr));
        UpdateEntry<PageTableEntrySv39>(entryAddr2, isWrite);

        PhysicalAddressSv39 paddr(
            entry2.GetMember<PageTableEntrySv39::PPN2>(),
            entry2.GetMember<PageTableEntrySv39::PPN1>(),
            vaddr.GetMember<VirtualAddressSv39::VPN0>(),
            vaddr.GetMember<VirtualAddressSv39::Offset>());
        *pOutAddr = ZeroExtend(39, paddr.GetValue());
        return std::nullopt;
    }

    PhysicalAddressSv39 entryAddr3(
        entry2.GetMember<PageTableEntrySv39::PPN2>(),
        entry2.GetMember<PageTableEntrySv39::PPN1>(),
        entry2.GetMember<PageTableEntrySv39::PPN0>(),
        sizeof(PageTableEntrySv39) * vaddr.GetMember<VirtualAddressSv39::VPN0>());
    const auto entry3 = PageTableEntrySv39(m_pBus->ReadUInt64(entryAddr3));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry3, accessType, pc, addr));
    RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry3, accessType, pc, addr));
    UpdateEntry<PageTableEntrySv39>(entryAddr3, isWrite);

    PhysicalAddressSv39 paddr(
        entry3.GetMember<PageTableEntrySv39::PPN2>(),
        entry3.GetMember<PageTableEntrySv39::PPN1>(),
        entry3.GetMember<PageTableEntrySv39::PPN0>(),
        vaddr.GetMember<VirtualAddressSv39::Offset>());
    *pOutAddr = ZeroExtend(39, paddr.GetValue());
    return std::nullopt;
}

std::optional<Trap> MemoryAccessUnit::TranslateSv48(paddr_t* pOutAddr, MemoryAccessType accessType, vaddr_t addr, vaddr_t pc)
{
    const bool isWrite = (accessType == (MemoryAccessType::Store));

    const auto vaddr = VirtualAddressSv48(addr);
    const auto satp = m_pCsr->ReadSatp();

    PhysicalAddressSv48 entryAddr1(
        satp.GetMember<satp_t::PPN_RV64>(),
        sizeof(PageTableEntrySv48) * vaddr.GetMember<VirtualAddressSv48::VPN3>());
    const auto entry1 = PageTableEntrySv48(m_pBus->ReadUInt64(entryAddr1));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry1, accessType, pc, addr));
    if (IsLeafEntry(entry1))
    {
        RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry1, accessType, pc, addr));
        UpdateEntry<PageTableEntrySv48>(entryAddr1, isWrite);

        PhysicalAddressSv48 paddr(
            entry1.GetMember<PageTableEntrySv48::PPN3>(),
            vaddr.GetMember<VirtualAddressSv48::VPN2>(),
            vaddr.GetMember<VirtualAddressSv48::VPN1>(),
            vaddr.GetMember<VirtualAddressSv48::VPN0>(),
            vaddr.GetMember<VirtualAddressSv48::Offset>());
        *pOutAddr = ZeroExtend(48, paddr.GetValue());
        return std::nullopt;
    }

    PhysicalAddressSv48 entryAddr2(
        entry1.GetMember<PageTableEntrySv48::PPN3>(),
        entry1.GetMember<PageTableEntrySv48::PPN2>(),
        entry1.GetMember<PageTableEntrySv48::PPN1>(),
        entry1.GetMember<PageTableEntrySv48::PPN0>(),
        sizeof(PageTableEntrySv48) * vaddr.GetMember<VirtualAddressSv48::VPN2>());
    const auto entry2 = PageTableEntrySv48(m_pBus->ReadUInt64(entryAddr2));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry2, accessType, pc, addr));
    if (IsLeafEntry(entry2))
    {
        RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry2, accessType, pc, addr));
        UpdateEntry<PageTableEntrySv48>(entryAddr2, isWrite);

        PhysicalAddressSv48 paddr(
            entry2.GetMember<PageTableEntrySv48::PPN3>(),
            entry2.GetMember<PageTableEntrySv48::PPN2>(),
            vaddr.GetMember<VirtualAddressSv48::VPN1>(),
            vaddr.GetMember<VirtualAddressSv48::VPN0>(),
            vaddr.GetMember<VirtualAddressSv48::Offset>());
        *pOutAddr = ZeroExtend(48, paddr.GetValue());
        return std::nullopt;
    }

    PhysicalAddressSv48 entryAddr3(
        entry2.GetMember<PageTableEntrySv48::PPN3>(),
        entry2.GetMember<PageTableEntrySv48::PPN2>(),
        entry2.GetMember<PageTableEntrySv48::PPN1>(),
        entry2.GetMember<PageTableEntrySv48::PPN0>(),
        sizeof(PageTableEntrySv48) * vaddr.GetMember<VirtualAddressSv48::VPN1>());
    const auto entry3 = PageTableEntrySv48(m_pBus->ReadUInt64(entryAddr3));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry3, accessType, pc, addr));
    if (IsLeafEntry(entry3))
    {
        RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry3, accessType, pc, addr));
        UpdateEntry<PageTableEntrySv48>(entryAddr3, isWrite);

        PhysicalAddressSv48 paddr(
            entry3.GetMember<PageTableEntrySv48::PPN3>(),
            entry3.GetMember<PageTableEntrySv48::PPN2>(),
            entry3.GetMember<PageTableEntrySv48::PPN1>(),
            vaddr.GetMember<VirtualAddressSv48::VPN0>(),
            vaddr.GetMember<VirtualAddressSv48::Offset>());
        *pOutAddr = ZeroExtend(48, paddr.GetValue());
        return std::nullopt;
    }

    PhysicalAddressSv48 entryAddr4(
        entry3.GetMember<PageTableEntrySv48::PPN3>(),
        entry3.GetMember<PageTableEntrySv48::PPN2>(),
        entry3.GetMember<PageTableEntrySv48::PPN1>(),
        entry3.GetMember<PageTableEntrySv48::PPN0>(),
        sizeof(PageTableEntrySv48) * vaddr.GetMember<VirtualAddressSv48::VPN0>());
    const auto entry4 = PageTableEntrySv48(m_pBus->ReadUInt64(entryAddr4));

    RAFI_RETURN_IF_TRAP(CheckTrapForEntry(entry4, accessType, pc, addr));
    RAFI_RETURN_IF_TRAP(CheckTrapForLeafEntry(entry4, accessType, pc, addr));
    UpdateEntry<PageTableEntrySv48>(entryAddr4, isWrite);

    PhysicalAddressSv48 paddr(
        entry4.GetMember<PageTableEntrySv48::PPN3>(),
        entry4.GetMember<PageTableEntrySv48::PPN2>(),
        entry4.GetMember<PageTableEntrySv48::PPN1>(),
        entry4.GetMember<PageTableEntrySv48::PPN0>(),
        vaddr.GetMember<VirtualAddressSv48::Offset>());

    *pOutAddr = ZeroExtend(48, paddr.GetValue());
    return std::nullopt;
}

std::optional<Trap> MemoryAccessUnit::TranslateSv57(paddr_t* pOutAddr, MemoryAccessType accessType, vaddr_t addr, vaddr_t pc)
{
    (void)pOutAddr;
    (void)accessType;
    (void)addr;
    (void)pc;
    RAFI_EMU_NOT_IMPLEMENTED();
}

std::optional<Trap> MemoryAccessUnit::TranslateSv64(paddr_t* pOutAddr, MemoryAccessType accessType, vaddr_t addr, vaddr_t pc)
{
    (void)pOutAddr;
    (void)accessType;
    (void)addr;
    (void)pc;
    RAFI_EMU_NOT_IMPLEMENTED();
}

}}}
