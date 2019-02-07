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

#include <cassert>
#include <cstdint>

#include <rafi/emu.h>

#include "Csr.h"

namespace rafi { namespace emu { namespace cpu {

class InterruptController
{
public:
    InterruptController(Csr* pCsr);

    InterruptType GetInterruptType() const;

    bool IsRequested() const;

    void Update();

    void RegisterExternalInterruptSource(IInterruptSource* pInterruptSource);
    void RegisterTimerInterruptSource(IInterruptSource* pInterruptSource);

private:
    void UpdateCsr();

    Csr* m_pCsr { nullptr };
    IInterruptSource* m_pExternalInterruptSource { nullptr };
    IInterruptSource* m_pTimerInterruptSource { nullptr };

    bool m_IsRequested { false };
    InterruptType m_InterruptType;
};

}}}
