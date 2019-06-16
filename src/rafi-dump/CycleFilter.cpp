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

#include <algorithm>
#include <sstream>

#include <rafi/trace.h>

#include "CycleFilter.h"

namespace rafi { namespace dump {

bool DefaultFilter::Apply(const trace::ICycle* pCycle) const
{
    (void)pCycle;
    return true;
}

PcFilter::PcFilter(uint64_t address, bool isPhysical)
    : m_Address(address)
    , m_IsPhysical(isPhysical)
{
}

bool PcFilter::Apply(const trace::ICycle* pCycle) const
{
    assert(!m_IsPhysical);

    return pCycle->GetPc() == m_Address;
}

MemoryAccessFilter::MemoryAccessFilter(uint64_t address, bool isPhysical, bool checkLoad, bool checkStore)
    : m_Address(address)
    , m_IsPhysical(isPhysical)
    , m_CheckLoad(checkLoad)
    , m_CheckStore(checkStore)
{
}

bool MemoryAccessFilter::Apply(const trace::ICycle* pCycle) const
{
    const auto count = pCycle->GetMemoryEventCount();

    for (int i = 0; i < count; i++)
    {
        trace::NodeMemoryEvent e;
        pCycle->CopyMemoryEvent(&e, i);

        const auto address = m_IsPhysical ? e.paddr : e.vaddr;
        const bool addressIncluded = (address <= m_Address && m_Address < address + e.size);

        switch (e.accessType)
        {
        case MemoryAccessType::Instruction:
        case MemoryAccessType::Load:
            if (m_CheckLoad && addressIncluded)
            {
                return true;
            }
            break;
        case MemoryAccessType::Store:
            if (m_CheckStore && addressIncluded)
            {
                return true;
            }
            break;
        default:
            break;
        }
    }

    return false;
}

std::unique_ptr<IFilter> MakeFilter(const std::string& description)
{
    if (description.empty())
    {
        return std::unique_ptr<IFilter>(new DefaultFilter());
    }

    // Parse description as <command>:<value>
    const char delimiter = ':';

    std::string s(description);
    std::replace(s.begin(), s.end(), delimiter, ' ');
    std::stringstream ss(s);

    std::string command;
    uint64_t value;
    ss >> command >> std::hex >> value;

    if (command == "A")
    {
        return std::unique_ptr<IFilter>(new MemoryAccessFilter(value, false, true, true));
    }
    else if (command == "AP")
    {
        return std::unique_ptr<IFilter>(new MemoryAccessFilter(value, true, true, true));
    }
    else if (command == "L")
    {
        return std::unique_ptr<IFilter>(new MemoryAccessFilter(value, false, true, false));
    }
    else if (command == "LP")
    {
        return std::unique_ptr<IFilter>(new MemoryAccessFilter(value, true, true, false));
    }
    else if (command == "S")
    {
        return std::unique_ptr<IFilter>(new MemoryAccessFilter(value, false, false, true));
    }
    else if (command == "SP")
    {
        return std::unique_ptr<IFilter>(new MemoryAccessFilter(value, true, false, true));
    }
    else if (command == "P")
    {
        return std::unique_ptr<IFilter>(new PcFilter(value, false));
    }
    else
    {
        std::cerr << "Unknown command " << command << std::endl;
        std::exit(1);
    }
}

}}
