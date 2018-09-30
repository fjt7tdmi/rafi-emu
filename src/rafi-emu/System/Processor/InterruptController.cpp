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

bool InterruptController::IsRequested() const
{
    assert(m_pTimer != nullptr);
    assert(m_pUser != nullptr);

    return m_pTimer->IsRequested() || m_pUser->IsRequested();
}

void InterruptController::RegisterTimerInterruptSource(IInterruptSource* pInterruptSource)
{
    m_pTimer = pInterruptSource;
}

void InterruptController::RegisterUserInterruptSource(IInterruptSource* pInterruptSource)
{
    m_pUser = pInterruptSource;
}
