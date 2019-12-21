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

#include <sstream>

#include <rafi/trace.h>

#include "StubSystem.h"

namespace rafi { namespace test {

StubSystem::~StubSystem()
{
}

void StubSystem::ProcessCycle()
{
};

bool StubSystem::IsValidMemory(paddr_t, size_t) const
{
    return false;
};

void StubSystem::ReadMemory(void*, size_t, paddr_t)
{
};

void StubSystem::WriteMemory(const void*, size_t, paddr_t)
{
};

vaddr_t StubSystem::GetPc() const
{
    return 0;
};

void StubSystem::CopyIntReg(trace::NodeIntReg32*) const
{
};

void StubSystem::CopyIntReg(trace::NodeIntReg64*) const
{
};

}}
