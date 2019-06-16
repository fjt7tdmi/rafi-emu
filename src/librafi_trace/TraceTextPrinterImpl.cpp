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

#include "TraceTextPrinterImpl.h"

namespace rafi { namespace trace {

void TraceTextPrinterImpl::Print(const trace::ICycle* pCycle)
{
    PrintHeader(pCycle);
    PrintBasic(pCycle);
    PrintIntReg(pCycle);
    PrintFpReg(pCycle);
    PrintIoState(pCycle);
    PrintOpEvent(pCycle);
    PrintMemoryEvent(pCycle);
    PrintTrapEvent(pCycle);
    PrintBreak();

    m_Cycle++;
}

void TraceTextPrinterImpl::PrintHeader(const trace::ICycle* pCycle) const
{
    if (m_Cycle == 0)
    {
        return;
    }

    if (pCycle->GetXLEN() == XLEN::XLEN32)
    {
        printf("XLEN  32\n");
    }
    else if (pCycle->GetXLEN() == XLEN::XLEN64)
    {
        printf("XLEN  64\n");
    }
    else
    {
        fprintf(stderr, "Unexpected XLEN.\n");
        std::exit(1);
    }
}

void TraceTextPrinterImpl::PrintBasic(const trace::ICycle* pCycle) const
{
    printf("BASIC %08" PRIx32 " %" PRIx32 " %016" PRIx64 " 0\n", pCycle->GetCycle(), static_cast<uint32_t>(pCycle->GetXLEN()), pCycle->GetPc());
}

void TraceTextPrinterImpl::PrintIntReg(const trace::ICycle* pCycle) const
{
    if (!pCycle->IsIntRegExist())
    {
        return;
    }

    printf(
        "INT\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n",
        pCycle->GetIntReg(0), pCycle->GetIntReg(1), pCycle->GetIntReg(2), pCycle->GetIntReg(3),
        pCycle->GetIntReg(4), pCycle->GetIntReg(5), pCycle->GetIntReg(6), pCycle->GetIntReg(7),
        pCycle->GetIntReg(8), pCycle->GetIntReg(9), pCycle->GetIntReg(10), pCycle->GetIntReg(11),
        pCycle->GetIntReg(12), pCycle->GetIntReg(13), pCycle->GetIntReg(14), pCycle->GetIntReg(15),
        pCycle->GetIntReg(16), pCycle->GetIntReg(17), pCycle->GetIntReg(18), pCycle->GetIntReg(19),
        pCycle->GetIntReg(20), pCycle->GetIntReg(21), pCycle->GetIntReg(22), pCycle->GetIntReg(23),
        pCycle->GetIntReg(24), pCycle->GetIntReg(25), pCycle->GetIntReg(26), pCycle->GetIntReg(27),
        pCycle->GetIntReg(28), pCycle->GetIntReg(29), pCycle->GetIntReg(30), pCycle->GetIntReg(31));
}

void TraceTextPrinterImpl::PrintFpReg(const trace::ICycle* pCycle) const
{
    if (!pCycle->IsFpRegExist())
    {
        return;
    }

    printf(
        "FP\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n"
        " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n",
        pCycle->GetFpReg(0), pCycle->GetFpReg(1), pCycle->GetFpReg(2), pCycle->GetFpReg(3),
        pCycle->GetFpReg(4), pCycle->GetFpReg(5), pCycle->GetFpReg(6), pCycle->GetFpReg(7),
        pCycle->GetFpReg(8), pCycle->GetFpReg(9), pCycle->GetFpReg(10), pCycle->GetFpReg(11),
        pCycle->GetFpReg(12), pCycle->GetFpReg(13), pCycle->GetFpReg(14), pCycle->GetFpReg(15),
        pCycle->GetFpReg(16), pCycle->GetFpReg(17), pCycle->GetFpReg(18), pCycle->GetFpReg(19),
        pCycle->GetFpReg(20), pCycle->GetFpReg(21), pCycle->GetFpReg(22), pCycle->GetFpReg(23),
        pCycle->GetFpReg(24), pCycle->GetFpReg(25), pCycle->GetFpReg(26), pCycle->GetFpReg(27),
        pCycle->GetFpReg(28), pCycle->GetFpReg(29), pCycle->GetFpReg(30), pCycle->GetFpReg(31));
}

void TraceTextPrinterImpl::PrintIoState(const trace::ICycle* pCycle) const
{
    if (!pCycle->IsIoExist())
    {
        return;
    }

    trace::NodeIo state;
    pCycle->CopyIo(&state);

    printf("IO %08" PRIx32 "\n", state.hostIo);
}

void TraceTextPrinterImpl::PrintOpEvent(const trace::ICycle* pCycle) const
{
    for (int i = 0; i < pCycle->GetOpEventCount(); i++)
    {
        trace::NodeOpEvent e;
        pCycle->CopyOpEvent(&e, i);

        printf("OP %" PRIx32 " %s\n", e.insn, GetString(e.priv));
    }
}

void TraceTextPrinterImpl::PrintMemoryEvent(const trace::ICycle* pCycle) const
{
    for (int i = 0; i < pCycle->GetMemoryEventCount(); i++)
    {
        trace::NodeMemoryEvent e;
        pCycle->CopyMemoryEvent(&e, i);

        printf("MA %s %" PRIx32 " %" PRIx64 " %" PRIx64 " %" PRIx64 "\n",
            GetString(e.accessType), e.size, e.value, e.vaddr, e.paddr);
    }
}

void TraceTextPrinterImpl::PrintTrapEvent(const trace::ICycle* pCycle) const
{
    for (int i = 0; i < pCycle->GetTrapEventCount(); i++)
    {
        trace::NodeTrapEvent e;
        pCycle->CopyTrapEvent(&e, i);

        printf("TRAP %s %s %s %" PRIx32 " %" PRIx64 "\n",
            GetString(e.trapType), GetString(e.from), GetString(e.to), e.cause, e.trapValue);
    }
}

void TraceTextPrinterImpl::PrintBreak() const
{
    printf("BREAK\n");
}

}}
