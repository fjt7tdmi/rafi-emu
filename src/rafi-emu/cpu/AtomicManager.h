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

#include <rafi/emu.h>

namespace rafi { namespace emu { namespace cpu {

class AtomicManager
{
public:
    bool IsReserved() const;

    vaddr_t GetReservedAddress() const;

    void Reserve(vaddr_t addr);

    void Cancel();
private:
    vaddr_t m_Address{0};
    bool m_Reserved{false};
};

}}}
