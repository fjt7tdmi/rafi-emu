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

#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <rafi/trace.h>

namespace rafi { namespace trace {

class TextCycle : ICycle
{
public:
    static std::unique_ptr<TextCycle> Parse(std::ifstream& input);

    TextCycle(XLEN xlen);
    virtual ~TextCycle();

    bool IsNodeExist(NodeType nodeType) const;

private:
    void ParseIntReg32Node(std::ifstream& input);
    void ParseIntReg64Node(std::ifstream& input);
    void ParsePc32Node(std::ifstream& input);
    void ParsePc64Node(std::ifstream& input);

    std::vector<IntReg32Node> m_IntReg32;
    std::vector<IntReg64Node> m_IntReg64;
    std::vector<Pc32Node> m_Pc32;
    std::vector<Pc64Node> m_Pc64;
};

}}
