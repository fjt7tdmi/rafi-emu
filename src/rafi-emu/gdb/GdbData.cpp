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

#include <string>

#include <rafi/emu.h>

#include "GdbCommandFactory.h"
#include "GdbException.h"

namespace rafi { namespace emu {

void GdbData::PushMemoryValue(paddr_t addr, uint64_t value)
{
    m_MemoryValues.emplace(addr, value);
}

uint64_t GdbData::PopMemoryValue(paddr_t addr)
{
    const auto value = m_MemoryValues[addr];

    m_MemoryValues.erase(addr);

    return value;
}

}}
