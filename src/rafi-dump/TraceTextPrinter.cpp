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

#include <cinttypes>
#include <cstring>

#include <rafi/trace.h>

#include "TraceTextPrinter.h"

namespace rafi { namespace dump {

void TraceTextPrinter::PrintCycle(const trace::ICycle* cycle)
{
    if (m_Cycle == 0)
    {
        PrintHeader(cycle);
    }

    printf(
        "NOTE  cycle%" PRId64 "\n"
        "PC  %016" PRIx64 "  %016" PRIx64 "\n"
        "INT\n"
        "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "\n"
        "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "\n"
        "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "\n"
        "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "\n"
        "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "\n"
        "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "\n"
        "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "\n"
        "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "  %016" PRIx64 "\n"
        "BREAK\n",
        m_Cycle,
        cycle->GetPc(false), cycle->GetPc(true),
        cycle->GetIntReg(0), cycle->GetIntReg(1), cycle->GetIntReg(2), cycle->GetIntReg(3),
        cycle->GetIntReg(4), cycle->GetIntReg(5), cycle->GetIntReg(6), cycle->GetIntReg(7),
        cycle->GetIntReg(8), cycle->GetIntReg(9), cycle->GetIntReg(10), cycle->GetIntReg(11),
        cycle->GetIntReg(12), cycle->GetIntReg(13), cycle->GetIntReg(14), cycle->GetIntReg(15),
        cycle->GetIntReg(16), cycle->GetIntReg(17), cycle->GetIntReg(18), cycle->GetIntReg(19),
        cycle->GetIntReg(20), cycle->GetIntReg(21), cycle->GetIntReg(22), cycle->GetIntReg(23),
        cycle->GetIntReg(24), cycle->GetIntReg(25), cycle->GetIntReg(26), cycle->GetIntReg(27),
        cycle->GetIntReg(28), cycle->GetIntReg(29), cycle->GetIntReg(30), cycle->GetIntReg(31));

    m_Cycle++;
}

void TraceTextPrinter::PrintHeader(const trace::ICycle* cycle)
{
    if (cycle->GetXLEN() == XLEN::XLEN32)
    {
        printf("XLEN  32\n");
    }
    else if (cycle->GetXLEN() == XLEN::XLEN64)
    {
        printf("XLEN  64\n");
    }
    else
    {
        fprintf(stderr, "Unexpected XLEN.\n");
        std::exit(1);
    }
}

}}
