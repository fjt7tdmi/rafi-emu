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

#include <string>

#include <rafi/trace.h>

namespace rafi {

class CommandLineOption
{
public:
    CommandLineOption(int argc, char** argv);

    const std::string& GetExpectPath() const;
    const std::string& GetActualPath() const;

    int GetCycleCount() const;
    int GetThreshold() const;

    bool CheckPhysicalPc() const;

private:
    static const int DefaultCycleCount = 1000 * 1000 * 1000;
    static const int DefaultThreshold = 10;

    std::string m_ExpectPath;
    std::string m_ActualPath;

    int m_CycleCount{ 0 };
    int m_Threshold{ 0 };
    bool m_CheckPhysicalPc{ false };
};

}
