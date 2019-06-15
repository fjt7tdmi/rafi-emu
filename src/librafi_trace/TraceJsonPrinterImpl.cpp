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

#include "TraceJsonPrinterImpl.h"

namespace rafi { namespace trace {

namespace {

    const char* GetCauseString(TrapType trapType, uint32_t cause)
    {
        switch (trapType)
        {
        case TrapType::Exception:
            return GetString(static_cast<ExceptionType>(cause));
        case TrapType::Interrupt:
            return GetString(static_cast<InterruptType>(cause));
        default:
            return "";
        }
    }

}

void TraceJsonPrinterImpl::Print(const trace::ICycle* pCycle)
{
    printf("{\n");

    PrintNote(pCycle);
    PrintPc(pCycle);
    PrintIntReg(pCycle);
    PrintFpReg(pCycle);
    PrintIoState(pCycle);
    PrintOpEvent(pCycle);
    PrintMemoryEvent(pCycle);
    PrintTrapEvent(pCycle);

    printf("}\n");

    m_Cycle++;
}

void TraceJsonPrinterImpl::PrintNote(const trace::ICycle* pCycle) const
{
    if (!pCycle->IsNoteExist())
    {
        return;
    }

    std::string s;
    pCycle->CopyNote(&s);

    printf("Note: %s\n", s.c_str());
}

void TraceJsonPrinterImpl::PrintPc(const trace::ICycle* pCycle) const
{
    if (!pCycle->IsPcExist())
    {
        return;
    }

    printf(
        "  Pc {\n"
        "    vaddr: 0x%" PRIx64 "\n"
        "    paddr: 0x%" PRIx64 "\n"
        "  }\n",
        pCycle->GetPc(false),
        pCycle->GetPc(true)
    );
}

void TraceJsonPrinterImpl::PrintIntReg(const trace::ICycle* pCycle) const
{
    if (!pCycle->IsIntRegExist())
    {
        return;
    }

    printf(
        "  IntReg: {\n"
        "    x0:  0x%" PRIx64 " // zero\n"
        "    x1:  0x%" PRIx64 " // ra\n"
        "    x2:  0x%" PRIx64 " // sp\n"
        "    x3:  0x%" PRIx64 " // gp\n"
        "    x4:  0x%" PRIx64 " // tp\n"
        "    x5:  0x%" PRIx64 " // t0\n"
        "    x6:  0x%" PRIx64 " // t1\n"
        "    x7:  0x%" PRIx64 " // t2\n"
        "    x8:  0x%" PRIx64 " // s0 (fp)\n"
        "    x9:  0x%" PRIx64 " // s1\n"
        "    x10: 0x%" PRIx64 " // a0\n"
        "    x11: 0x%" PRIx64 " // a1\n"
        "    x12: 0x%" PRIx64 " // a2\n"
        "    x13: 0x%" PRIx64 " // a3\n"
        "    x14: 0x%" PRIx64 " // a4\n"
        "    x15: 0x%" PRIx64 " // a5\n"
        "    x16: 0x%" PRIx64 " // a6\n"
        "    x17: 0x%" PRIx64 " // a7\n"
        "    x18: 0x%" PRIx64 " // s2\n"
        "    x19: 0x%" PRIx64 " // s3\n"
        "    x20: 0x%" PRIx64 " // s4\n"
        "    x21: 0x%" PRIx64 " // s5\n"
        "    x22: 0x%" PRIx64 " // s6\n"
        "    x23: 0x%" PRIx64 " // s7\n"
        "    x24: 0x%" PRIx64 " // s8\n"
        "    x25: 0x%" PRIx64 " // s9\n"
        "    x26: 0x%" PRIx64 " // s10\n"
        "    x27: 0x%" PRIx64 " // s11\n"
        "    x28: 0x%" PRIx64 " // t3\n"
        "    x29: 0x%" PRIx64 " // t4\n"
        "    x30: 0x%" PRIx64 " // t5\n"
        "    x31: 0x%" PRIx64 " // t6\n"
        "  }\n",
        pCycle->GetIntReg(0),
        pCycle->GetIntReg(1),
        pCycle->GetIntReg(2),
        pCycle->GetIntReg(3),
        pCycle->GetIntReg(4),
        pCycle->GetIntReg(5),
        pCycle->GetIntReg(6),
        pCycle->GetIntReg(7),
        pCycle->GetIntReg(8),
        pCycle->GetIntReg(9),
        pCycle->GetIntReg(10),
        pCycle->GetIntReg(11),
        pCycle->GetIntReg(12),
        pCycle->GetIntReg(13),
        pCycle->GetIntReg(14),
        pCycle->GetIntReg(15),
        pCycle->GetIntReg(16),
        pCycle->GetIntReg(17),
        pCycle->GetIntReg(18),
        pCycle->GetIntReg(19),
        pCycle->GetIntReg(20),
        pCycle->GetIntReg(21),
        pCycle->GetIntReg(22),
        pCycle->GetIntReg(23),
        pCycle->GetIntReg(24),
        pCycle->GetIntReg(25),
        pCycle->GetIntReg(26),
        pCycle->GetIntReg(27),
        pCycle->GetIntReg(28),
        pCycle->GetIntReg(29),
        pCycle->GetIntReg(30),
        pCycle->GetIntReg(31));
}

void TraceJsonPrinterImpl::PrintFpReg(const trace::ICycle* pCycle) const
{
    if (!pCycle->IsFpRegExist())
    {
        return;
    }

    printf("  FpReg: {\n");

    for (int i = 0; i < 32; i++)
    {
        FpRegNodeUnion u;
        u.u64.value = pCycle->GetFpReg(i);

        printf(
            "    f%-2d: { u64: 0x%016llx, f32: %e, f64: %e } // %s\n",
            i,
            u.u64.value,
            u.f32.value,
            u.f64.value,
            GetFpRegName(i)
        );
    }

    printf("  }\n");
}

void TraceJsonPrinterImpl::PrintIoState(const trace::ICycle* pCycle) const
{
    if (!pCycle->IsIoStateExist())
    {
        return;
    }

    trace::IoState state;
    pCycle->CopyIoState(&state);

    printf(
        "  Io {\n"
        "    host: 0x%" PRIx32 "\n"
        "  }\n",
        state.hostIo
    );
}

void TraceJsonPrinterImpl::PrintOpEvent(const trace::ICycle* pCycle) const
{
    Decoder decoder(pCycle->GetXLEN());

    for (int i = 0; i < pCycle->GetOpEventCount(); i++)
    {
        OpEvent e;
        pCycle->CopyOpEvent(&e, i);

        auto op = decoder.Decode(e.insn);

        char opStr[64];
        SNPrintOp(opStr, sizeof(opStr), op);

        printf(
            "  Op {\n"
            "    insn:  0x%" PRIx32 " // %s\n"
            "    priv:  %s\n"
            "  }\n",
            e.insn,
            opStr,
            GetString(e.priv)
        );
    }
}

void TraceJsonPrinterImpl::PrintMemoryEvent(const trace::ICycle* pCycle) const
{
    for (int i = 0; i < pCycle->GetMemoryEventCount(); i++)
    {
        trace::MemoryEvent e;
        pCycle->CopyMemoryEvent(&e, i);

        printf(
            "  MemoryAccess {\n"
            "    accessType: %s\n"
            "    size: %d // byte\n"
            "    value: 0x%" PRIx64 "\n"
            "    vaddr: 0x%" PRIx64 "\n"
            "    paddr: 0x%" PRIx64 "\n"
            "  }\n",
            GetString(e.accessType),
            e.size,
            static_cast<unsigned long long>(e.value),
            static_cast<unsigned long long>(e.virtualAddress),
            static_cast<unsigned long long>(e.physicalAddress)
        );
    }
}

void TraceJsonPrinterImpl::PrintTrapEvent(const trace::ICycle* pCycle) const
{
    for (int i = 0; i < pCycle->GetTrapEventCount(); i++)
    {
        trace::TrapEvent e;
        pCycle->CopyTrapEvent(&e, i);

        printf(
            "  Trap {\n"
            "    type:  %s\n"
            "    from:  %s\n"
            "    to:    %s\n"
            "    cause: %s\n"
            "    trapValue: 0x%" PRIx64 "\n"
            "  }\n",
            GetString(e.trapType),
            GetString(e.from),
            GetString(e.to),
            GetCauseString(e.trapType, e.cause),
            e.trapValue
        );
    }
}

}}
