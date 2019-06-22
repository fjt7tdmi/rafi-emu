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

#include <memory>
#include <string>

#include <rafi/trace.h>

namespace rafi { namespace dump {

class IFilter
{
public:
    virtual ~IFilter(){}
    virtual bool Apply(const trace::ICycle* pCycle) const = 0;
};

class DefaultFilter : public IFilter
{
public:
    virtual bool Apply(const trace::ICycle* pCycle) const override;
};

class PcFilter : public IFilter
{
public:
    PcFilter(uint64_t address, bool isPhysical);

    virtual bool Apply(const trace::ICycle* pCycle) const override;

private:
    uint64_t m_Address;
    bool m_IsPhysical;
};

class MemoryAccessFilter : public IFilter
{
public:
    MemoryAccessFilter(uint64_t address, bool isPhysical, bool checkLoad, bool checkStore);

    virtual bool Apply(const trace::ICycle* pCycle) const override;

private:
    uint64_t m_Address{ 0 };
    bool m_IsPhysical{ false };
    bool m_CheckLoad{ false };
    bool m_CheckStore{ false };
};

std::unique_ptr<IFilter> MakeFilter(const std::string& description);

}}
