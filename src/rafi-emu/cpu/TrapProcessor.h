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

#include <cstring>

#include <rafi/BitField.h>
#include <rafi/Event.h>

#include "Csr.h"
#include "CsrTypes.h"
#include "Trap.h"

using namespace rvtrace;

namespace rafi { namespace emu { namespace cpu {

class TrapProcessor
{
public:
    explicit TrapProcessor(Csr* pCsr)
        : m_pCsr(pCsr)
	{
	}

    void ProcessException(const Trap& trap);
    void ProcessInterrupt(InterruptType type, int32_t pc);
    void ProcessTrapReturn(PrivilegeLevel level);

    // for Dump
    void ClearEvent();
    void CopyTrapEvent(TrapEvent* pOut) const;
    bool IsTrapEventExist() const;

private:
    void ProcessTrapEnter(bool isInterrupt, int32_t exceptionCode, int32_t trapValue, int32_t pc, PrivilegeLevel nextPrivilegeLevel);

    Csr* m_pCsr;

    bool m_TrapEventValid { false };

    TrapEvent m_TrapEvent;
};

}}}
