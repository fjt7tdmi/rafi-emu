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

#include "../util/TraceUtil.h"

namespace rafi { namespace dump {

class CommandLineOption
{
public:
    CommandLineOption(int argc, char** argv);

    PrinterType GetPrinterType() const;

    const std::string& GetFilterDescription() const;
    const std::string& GetPath() const;

    const int GetCycleBegin() const;
    const int GetCycleCount() const;
    const int GetCycleEnd() const;

private:
    PrinterType m_PrinterType;

    std::string m_FilterDescription;
    std::string m_Path;

    int m_CycleBegin;
    int m_CycleCount;
    int m_CycleEnd;
};

}}
