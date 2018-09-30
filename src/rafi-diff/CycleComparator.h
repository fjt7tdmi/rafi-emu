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

#include <rvtrace/reader.h>

class CycleComparator final
{
public:
    CycleComparator(bool cmpPhysicalPc, bool cmpCsr, bool cmpMemory)
        : m_CmpPhysicalPc(cmpPhysicalPc)
        , m_CmpCsr(cmpCsr)
        , m_CmpMemory(cmpMemory)
    {
    }

    // compare
    bool IsPc32NodeMatched(const rvtrace::Pc32Node* pExpect, const rvtrace::Pc32Node* pActual) const;
    bool IsIntReg32NodeMatched(const rvtrace::IntReg32Node* pExpect, const rvtrace::IntReg32Node* pActual) const;
    bool IsCsr32NodeMatched(const rvtrace::Csr32NodeHeader* pExpect, const rvtrace::Csr32NodeHeader* pActual) const;
    bool IsMemoryNodeMatched(const rvtrace::MemoryNodeHeader* pExpect, const rvtrace::MemoryNodeHeader* pActual) const;

    bool AreMatched(const rvtrace::TraceCycleReader& expect, const rvtrace::TraceCycleReader& actual) const;

    // print diff
    void PrintPc32Diff(const rvtrace::Pc32Node* pExpect, const rvtrace::Pc32Node* pActual) const;
    void PrintIntReg32Diff(const rvtrace::IntReg32Node* pExpect, const rvtrace::IntReg32Node* pActual) const;
    void PrintCsr32Diff(const rvtrace::Csr32NodeHeader* pExpect, const rvtrace::Csr32NodeHeader* pActual) const;
    void PrintMemoryDiff(const rvtrace::MemoryNodeHeader* pExpect, const rvtrace::MemoryNodeHeader* pActual) const;

    void PrintDiff(const rvtrace::TraceCycleReader& expect, const rvtrace::TraceCycleReader& actual) const;

private:
    bool m_CmpPhysicalPc;
    bool m_CmpCsr;
    bool m_CmpMemory;
};
