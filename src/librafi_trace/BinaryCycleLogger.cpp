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

#include <cstring>

#include <rafi/trace.h>

#include "BinaryCycleLoggerImpl.h"

namespace rafi { namespace trace {

BinaryCycleLogger::BinaryCycleLogger(uint32_t cycle, const XLEN& xlen, uint64_t pc)
{
    m_pImpl = new BinaryCycleLoggerImpl(cycle, xlen, pc);
}

BinaryCycleLogger::~BinaryCycleLogger()
{
    delete m_pImpl;
}

void BinaryCycleLogger::Add(const NodeIntReg32& value)
{
    m_pImpl->Add(value);
}

void BinaryCycleLogger::Add(const NodeIntReg64& value)
{
    m_pImpl->Add(value);
}

void BinaryCycleLogger::Add(const NodeFpReg& value)
{
    m_pImpl->Add(value);
}

void BinaryCycleLogger::Add(const NodeIo& value)
{
    m_pImpl->Add(value);
}

void BinaryCycleLogger::Add(const NodeOpEvent& value)
{
    m_pImpl->Add(value);
}

void BinaryCycleLogger::Add(const NodeTrapEvent& value)
{
    m_pImpl->Add(value);
}

void BinaryCycleLogger::Add(const NodeMemoryEvent& value)
{
    m_pImpl->Add(value);
}

void BinaryCycleLogger::Break()
{
    m_pImpl->Break();
}

void* BinaryCycleLogger::GetData()
{
    return m_pImpl->GetData();
}

size_t BinaryCycleLogger::GetDataSize() const
{
    return m_pImpl->GetDataSize();
}

}}
