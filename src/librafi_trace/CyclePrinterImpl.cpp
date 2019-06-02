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

#include <rafi/trace.h>

#include "CyclePrinterImpl.h"

namespace rafi { namespace trace {

void CyclePrinterImpl::Print(const ICycle* pCycle)
{
    PrintPc(pCycle);
}

void CyclePrinterImpl::Enable(NodeType nodeType)
{
    switch (nodeType)
    {
    case NodeType::Pc32:
    case NodeType::Pc64:
        m_EnablePc = true;
        break;
    default:
        break;
    }
}

void CyclePrinterImpl::PrintPc(const ICycle* pCycle)
{
    printf(
        "PC %016" PRIx64 " %016" PRIx64 "\n",
        pCycle->GetPc(false), pCycle->GetPc(true)
    );
}

}}
