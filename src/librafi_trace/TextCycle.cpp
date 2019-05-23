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

#include <rafi/trace.h>

#include "TextCycle.h"

namespace rafi { namespace trace {

std::unique_ptr<TextCycle> TextCycle::Parse(std::ifstream& input)
{
    auto p = std::make_unique<TextCycle>();

    std::string s;

    for (;;)
    {
        input >> s;

        if (s == "CYCLE")
        {
            break;
        }
        else if (s == "GPR32")
        {
            p->ParseIntReg32Node(input);
        }
        else if (s == "GPR64")
        {
            p->ParseIntReg64Node(input);
        }
        else if (s == "PC32")
        {
            p->ParseIntReg32Node(input);
        }
        else if (s == "PC64")
        {
            p->ParseIntReg64Node(input);
        }
        else
        {
            throw TraceException("Trace text parse error: unknown literal.");
        }
    }
}

CycleConfig TextCycle::GetCycleConfig() const
{
    CycleConfig config;

    config.SetNodeCount(NodeType::IntReg32, m_IntReg32.size());
    config.SetNodeCount(NodeType::IntReg64, m_IntReg64.size());
    config.SetNodeCount(NodeType::Pc32, m_Pc32.size());
    config.SetNodeCount(NodeType::Pc64, m_Pc64.size());

    return config;
}

const IntReg32Node& TextCycle::GetIntReg32Node() const
{
    return m_IntReg32[0];
}

const IntReg64Node& TextCycle::GetIntReg64Node() const
{
    return m_IntReg64[0];
}

const Pc32Node& TextCycle::GetPc32Node() const
{
    return m_Pc32[0];
}

const Pc64Node& TextCycle::GetPc64Node() const
{
    return m_Pc64[0];
}

bool IsNodeExist(NodeType nodeType) const
{
    return GetCycleConfig().nodeType)
}

void TextCycle::ParseIntReg32Node(std::ifstream& input)
{
    IntReg32Node node;

    for (int i = 0; i < 32; i++)
    {
        input >> std::hex >> node.regs[i];
    }

    m_IntReg32.push_back(node);
}

void TextCycle::ParseIntReg64Node(std::ifstream& input)
{
    IntReg64Node node;

    for (int i = 0; i < 32; i++)
    {
        input >> std::hex >> node.regs[i];
    }

    m_IntReg64.push_back(node);
}

void TextCycle::ParsePc32Node(std::ifstream& input)
{
    Pc32Node node;

    input >> std::hex >> node.virtualPc;
    node.physicalPc = 0;

    m_Pc32.push_back(node);
}

void TextCycle::ParsePc64Node(std::ifstream& input)
{
    Pc64Node node;

    input >> std::hex >> node.virtualPc;
    node.physicalPc = 0;

    m_Pc64.push_back(node);
}

}}
