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

#include <cassert>
#include <cstring>

#include "InterruptController.h"

namespace {
    int32_t CountBits(int32_t value)
    {
        auto tmp = value;

        tmp = (tmp & 0x55555555) + (tmp >> 1 & 0x55555555);
        tmp = (tmp & 0x33333333) + (tmp >> 2 & 0x33333333);
        tmp = (tmp & 0x0f0f0f0f) + (tmp >> 4 & 0x0f0f0f0f);
        tmp = (tmp & 0x00ff00ff) + (tmp >> 8 & 0x00ff00ff);
        tmp = (tmp & 0x0000ffff) + (tmp >> 16 & 0x0000ffff);

        return tmp;
    }

    int32_t NumberOfTrainingZero(int32_t value)
    {
        return CountBits((value & (-value)) - 1);
    }
}

InterruptController::InterruptController(Csr* pCsr)
    : m_pCsr(pCsr)
{
}

void InterruptController::Update()
{
    xie_t enable = m_pCsr->ReadInterruptEnable();
    xip_t pending = m_pCsr->ReadInterruptPending();

    const int32_t mask = enable.GetInt32();
    const int32_t value = static_cast<int32_t>(pending.GetWithMask(mask));

    m_IsRequested = (value != 0);
    m_InterruptType = static_cast<InterruptType>(NumberOfTrainingZero(value));
}

bool InterruptController::IsRequested() const
{
    return m_IsRequested;
}

InterruptType InterruptController::GetInterruptType() const
{
    assert(m_IsRequested);

    return m_InterruptType;
}
