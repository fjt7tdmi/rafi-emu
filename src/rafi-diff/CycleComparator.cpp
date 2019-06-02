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
#include <memory>
#include <string>
#include <sstream>

#include <rafi/trace.h>

#include "CycleComparator.h"

namespace rafi {

bool CycleComparator::IsPcMatched(const trace::ICycle* expect, const trace::ICycle* actual) const
{
    if (!expect->IsPcExist() || !actual->IsPcExist())
    {
        return false;
    }

    if (m_CmpPhysicalPc)
    {
        return (expect->GetPc(false) == actual->GetPc(false)) && (expect->GetPc(true) == actual->GetPc(true));
    }
    else
    {
        return (expect->GetPc(false) == actual->GetPc(false));
    }
}

bool CycleComparator::IsIntRegMatched(const trace::ICycle* expect, const trace::ICycle* actual) const
{
    if (!expect->IsIntRegExist() || !actual->IsIntRegExist())
    {
        return false;
    }

    for (int i = 0; i < IntRegCount; i++)
    {
        if (expect->GetIntReg(i) != actual->GetIntReg(i))
        {
            return false;
        }
    }

    return true;
}

bool CycleComparator::IsMatched(const trace::ICycle* expect, const trace::ICycle* actual) const
{
    if (!IsPcMatched(expect, actual))
    {
        return false;
    }

    if (!IsIntRegMatched(expect, actual))
    {
        return false;
    }

    return true;
}

void CycleComparator::PrintDiffPc(const trace::ICycle* expect, const trace::ICycle* actual) const
{
    if (!expect->IsPcExist())
    {
        printf("    - expect has no PC node.\n");
    }

    if (!actual->IsPcExist())
    {
        printf("    - actual has no PC node.\n");
    }

    if (expect->IsPcExist() && actual->IsPcExist())
    {
        if (expect->GetPc(false) != actual->GetPc(false))
        {
            printf("    - virtual PCs are not matched (expect:0x%" PRIx64 ", actual:0x%" PRIx64 ")\n", expect->GetPc(false), actual->GetPc(false));
        }
        if (m_CmpPhysicalPc && expect->GetPc(true) != actual->GetPc(true))
        {
            printf("    - physical PCs are not matched (expect:0x%" PRIx64 ", actual:0x%" PRIx64 ")\n", expect->GetPc(true), actual->GetPc(true));
        }
    }
}

void CycleComparator::PrintDiffIntReg(const trace::ICycle* expect, const trace::ICycle* actual) const
{
    if (!expect->IsIntRegExist())
    {
        printf("    - expect has no IntReg32Node.\n");
    }

    if (!actual->IsIntRegExist())
    {
        printf("    - actual has no IntReg32Node.\n");
    }

    if (expect->IsIntRegExist() && actual->IsIntRegExist())
    {
        for (int i = 0; i < 32; i++)
        {
            if (expect->GetIntReg(i) != actual->GetIntReg(i))
            {
                printf("    - x%d not matched (expect:0x%" PRIx64 ", actual:0x%" PRIx64 ")\n", i, expect->GetIntReg(i), actual->GetIntReg(i));
            }
        }
    }
}

void CycleComparator::PrintDiff(const trace::ICycle* expect, const trace::ICycle* actual) const
{
    if (!IsPcMatched(expect, actual))
    {
        PrintDiffPc(expect, actual);
        return;
    }

    if (!IsIntRegMatched(expect, actual))
    {
        PrintDiffIntReg(expect, actual);
    }
}

}
