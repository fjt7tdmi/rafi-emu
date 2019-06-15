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

#include <rafi/trace.h>

namespace rafi { namespace trace {

class TraceTextPrinter
{
public:
    void PrintCycle(const trace::ICycle* cycle);

private:
    void PrintHeader(const trace::ICycle* cycle) const;
    void PrintNote(const trace::ICycle* cycle) const;
    void PrintPc(const trace::ICycle* cycle) const;
    void PrintIntReg(const trace::ICycle* cycle) const;
    void PrintFpReg(const trace::ICycle* cycle) const;
    void PrintIoState(const trace::ICycle* cycle) const;
    void PrintOpEvent(const trace::ICycle* cycle) const;
    void PrintMemoryEvent(const trace::ICycle* cycle) const;
    void PrintTrapEvent(const trace::ICycle* cycle) const;
    void PrintBreak() const;

    uint64_t m_Cycle{ 0 };
};

}}
