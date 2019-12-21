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

#include <map>
#include <rafi/emu.h>

#include "GdbCommands.h"

namespace rafi { namespace emu {

class GdbData
{
public:
    void PushMemoryValue(paddr_t addr, uint64_t value);

    uint64_t PopMemoryValue(paddr_t addr);

private:
    std::map<paddr_t, uint64_t> m_MemoryValues;
};

}}
