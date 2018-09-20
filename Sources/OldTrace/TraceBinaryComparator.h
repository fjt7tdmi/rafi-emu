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

#include <cstdio>

#include "TraceBinary.h"

class TraceBinaryComparator final
{
public:
    TraceBinaryComparator(bool cmpPhysicalPc, bool cmpCsr, bool cmpMemory)
        : m_CmpPhysicalPc(cmpPhysicalPc)
        , m_CmpCsr(cmpCsr)
        , m_CmpMemory(cmpMemory)
    {
    }

    // compare
    bool IsPc32NodeMatched(Pc32Node* pExpect, Pc32Node* pActual) const;
    bool IsIntReg32NodeMatched(IntReg32Node* pExpect, IntReg32Node* pActual) const;
    bool IsCsr32NodeMatched(Csr32NodeHeader* pExpect, Csr32NodeHeader* pActual) const;
    bool IsMemoryNodeMatched(MemoryNodeHeader* pExpect, MemoryNodeHeader* pActual) const;
    bool AreTraceChildsMatched(char* pExpect, size_t expectSize, char* pActual, size_t actualSize) const;

    // print diff
    void PrintPc32Diff(Pc32Node* pExpect, Pc32Node* pActual) const;
    void PrintIntReg32Diff(IntReg32Node* pExpect, IntReg32Node* pActual) const;
    void PrintCsr32Diff(Csr32NodeHeader* pExpect, Csr32NodeHeader* pActual) const;
    void PrintMemoryDiff(MemoryNodeHeader* pExpect, MemoryNodeHeader* pActual) const;
    void PrintDiff(char* pExpect, size_t expectSize, char* pActual, size_t actualSize) const;

private:
    bool m_CmpPhysicalPc;
    bool m_CmpCsr;
    bool m_CmpMemory;
};
