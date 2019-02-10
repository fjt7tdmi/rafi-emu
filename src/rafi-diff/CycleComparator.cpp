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

#include <cstring>
#include <memory>
#include <string>
#include <sstream>

#include <rafi/trace.h>

#include "CycleComparator.h"

// Suppress VC warning for printf "%16lx"
#pragma warning(disable:4477)

using namespace rafi;
using namespace rafi::trace;

bool CycleComparator::IsPc32NodeMatched(const Pc32Node* pExpect, const Pc32Node* pActual) const
{
    if (pExpect == nullptr || pActual == nullptr)
    {
        return false;
    }

    if (m_CmpPhysicalPc)
    {
        return
            (pExpect->virtualPc == pActual->virtualPc) &&
            (pExpect->physicalPc == pActual->physicalPc);
    }
    else
    {
        return (pExpect->virtualPc == pActual->virtualPc);
    }
}

bool CycleComparator::IsIntReg32NodeMatched(const IntReg32Node* pExpect, const IntReg32Node* pActual) const
{
    if (pExpect == nullptr || pActual == nullptr)
    {
        return false;
    }

    return std::memcmp(pExpect->regs, pActual->regs, sizeof(pExpect->regs)) == 0;
}

bool CycleComparator::AreCsr32NodesMatched(const Csr32Node* pExpect, int expectCsr32Count, const Csr32Node* pActual, int actualCsr32Count) const
{
    if (pExpect == nullptr || pActual == nullptr)
    {
        return false;
    }

    if (expectCsr32Count != actualCsr32Count)
    {
        return false;
    }

    if (expectCsr32Count < 0)
    {
        throw TraceException("Detect minus node size.");
    }

    return std::memcmp(pExpect, pActual, expectCsr32Count * sizeof(Csr32Node)) == 0;
}

bool CycleComparator::IsMemoryNodeMatched(const void* pExpect, int64_t expectSize, const void* pActual, int64_t actualSize) const
{
    if (pExpect == nullptr || pActual == nullptr)
    {
        return false;
    }

    if (expectSize != actualSize)
    {
        return false;
    }

    if (expectSize < 0)
    {
        throw TraceException("Detect minus node size.");
    }

#if INT64_MAX > SIZE_MAX
    if (expectSize > SIZE_MAX)
    {
        throw TraceException("Detect too big node size.");
    }
#endif

    return std::memcmp(pExpect, pActual, static_cast<size_t>(expectSize)) == 0;
}

bool CycleComparator::AreMatched(const TraceCycleReader& expect, const TraceCycleReader& actual) const
{
    if (!IsPc32NodeMatched(expect.GetPc32Node(), actual.GetPc32Node()))
    {
        return false;
    }
    if (!IsIntReg32NodeMatched(expect.GetIntReg32Node(), actual.GetIntReg32Node()))
    {
        return false;
    }
    if (m_CmpCsr)
    {
        const auto expectCount = static_cast<int>(expect.GetNodeSize(NodeType::Csr32) / sizeof(Csr32Node));
        const auto actualCount = static_cast<int>(actual.GetNodeSize(NodeType::Csr32) / sizeof(Csr32Node));

        if (!AreCsr32NodesMatched(expect.GetCsr32Node(), expectCount, actual.GetCsr32Node(), actualCount))
        {
            return false;
        }
    }
    if (m_CmpMemory && !IsMemoryNodeMatched(expect.GetMemoryNode(), expect.GetNodeSize(NodeType::Memory), actual.GetMemoryNode(), actual.GetNodeSize(NodeType::Memory)))
    {
        return false;
    }
    return true;
}

void CycleComparator::PrintPc32Diff(const Pc32Node* pExpect, const Pc32Node* pActual) const
{
    if (pExpect == nullptr)
    {
        printf("    - expect has no Pc32Node.\n");
    }

    if (pActual == nullptr)
    {
        printf("    - actual has no Pc32Node.\n");
    }

    if (pExpect != nullptr && pActual != nullptr)
    {
        if (pExpect->virtualPc != pActual->virtualPc)
        {
            printf("    - virtualPc not matched (expect:0x%08x, actual:0x%08x)\n", pExpect->virtualPc, pActual->virtualPc);
        }
        if (m_CmpPhysicalPc && pExpect->physicalPc != pActual->physicalPc)
        {
            printf("    - physicalPc not matched (expect:0x%08x, actual:0x%08x)\n", pExpect->physicalPc, pActual->virtualPc);
        }
    }
}

void CycleComparator::PrintIntReg32Diff(const IntReg32Node* pExpect, const IntReg32Node* pActual) const
{
    if (pExpect == nullptr)
    {
        printf("    - expect has no IntReg32Node.\n");
    }

    if (pActual == nullptr)
    {
        printf("    - actual has no IntReg32Node.\n");
    }

    if (pExpect != nullptr && pActual != nullptr)
    {
        for (int i = 0; i < 32; i++)
        {
            if (pExpect->regs[i] != pActual->regs[i])
            {
                printf("    - x%d not matched (expect:0x%08x, actual:0x%08x)\n", i, pExpect->regs[i], pActual->regs[i]);
            }
        }
    }
}

void CycleComparator::PrintCsr32Diff(const Csr32Node* expectNodes, int expectNodeCount, const Csr32Node* actualNodes, int actualNodeCount) const
{
    if (expectNodes == nullptr || actualNodes == nullptr)
    {
        if (expectNodes == nullptr)
        {
            printf("    - expect has no Csr32Node.\n");
        }
        if (actualNodes == nullptr)
        {
            printf("    - actual has no Csr32Node.\n");
        }
        return;
    }

    if (expectNodeCount != actualNodeCount)
    {
        printf("    - Csr32Node counts are not matched (expect:%d, actual:%d).\n", expectNodeCount, actualNodeCount);
        return;
    }

    const int count = expectNodeCount;

    // Check addresses
    bool addressMatched = true;

    for (int i = 0; i < count; i++)
    {
        const auto expectAddress = static_cast<csr_addr_t>(expectNodes[i].address);
        const auto actualAddress = static_cast<csr_addr_t>(actualNodes[i].address);

        if (expectAddress != actualAddress)
        {
            printf("    - csr address is not matched (index=%d).\n", i);
            printf("      expect: %s (%d)\n", GetString(expectAddress), expectNodes[i].address);
            printf("      actual: %s (%d)\n", GetString(actualAddress), actualNodes[i].address);

            addressMatched = false;
        }
    }

    if (!addressMatched)
    {
        return;
    }

    // Check values
    for (int i = 0; i < count; i++)
    {
        const auto address = static_cast<csr_addr_t>(expectNodes[i].address);

        if (expectNodes[i].value != actualNodes[i].value)
        {
            printf("    - csr value is not matched (%s).\n", GetString(address));
            printf("      expect: %08x\n", expectNodes[i].value);
            printf("      actual: %08x\n", actualNodes[i].value);
        }
    }
}

void CycleComparator::PrintMemoryDiff(const void* pExpect, size_t expectSize, const void* pActual, size_t actualSize) const
{
    if (pExpect == nullptr || pActual == nullptr)
    {
        if (pExpect == nullptr)
        {
            printf("    - expect has no MemoryNode.\n");
        }
        if (pActual == nullptr)
        {
            printf("    - actual has no MemoryNode.\n");
        }
        return;
    }

    if (expectSize != actualSize)
    {
        printf("    - MemoryNode body size is not matched (expect:0x%016llx, actual:0x%016llx).\n", expectSize, actualSize);
        return;
    }

    const int64_t unitSize = 16;
    if (expectSize % unitSize != 0)
    {
        throw TraceException("MemoryNode body size must be multiple of 16.");
    }

    const auto count = expectSize / unitSize;

    const auto expectBody = reinterpret_cast<const uint8_t*>(pExpect);
    const auto actualBody = reinterpret_cast<const uint8_t*>(pActual);

    for (int64_t i = 0; i < count; i++)
    {
        const auto offset = i * unitSize;
        if (std::memcmp(&expectBody[offset], &actualBody[offset], unitSize) != 0)
        {
            printf("    - memory value is not matched (offset=0x%16lx).\n", offset);
            printf("      expect: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
                expectBody[offset +  0], expectBody[offset +  1], expectBody[offset +  2], expectBody[offset +  3],
                expectBody[offset +  4], expectBody[offset +  5], expectBody[offset +  6], expectBody[offset +  7],
                expectBody[offset +  8], expectBody[offset +  9], expectBody[offset + 10], expectBody[offset + 11],
                expectBody[offset + 12], expectBody[offset + 13], expectBody[offset + 14], expectBody[offset + 15]);
            printf("      actual: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
                actualBody[offset +  0], actualBody[offset +  1], actualBody[offset +  2], actualBody[offset +  3],
                actualBody[offset +  4], actualBody[offset +  5], actualBody[offset +  6], actualBody[offset +  7],
                actualBody[offset +  8], actualBody[offset +  9], actualBody[offset + 10], actualBody[offset + 11],
                actualBody[offset + 12], actualBody[offset + 13], actualBody[offset + 14], actualBody[offset + 15]);
        }
    }
}

void CycleComparator::PrintDiff(const TraceCycleReader& expect, const TraceCycleReader& actual) const
{
    const auto pExpectPc32 = expect.GetPc32Node();
    const auto pActualPc32 = actual.GetPc32Node();
    if (!IsPc32NodeMatched(pExpectPc32, pActualPc32))
    {
        PrintPc32Diff(pExpectPc32, pActualPc32);
    }

    const auto pExpectIntReg32 = expect.GetIntReg32Node();
    const auto pActualIntReg32 = actual.GetIntReg32Node();
    if (!IsIntReg32NodeMatched(pExpectIntReg32, pActualIntReg32))
    {
        PrintIntReg32Diff(pExpectIntReg32, pActualIntReg32);
    }

    const auto pExpectCsr32 = expect.GetCsr32Node();
    const auto pActualCsr32 = actual.GetCsr32Node();
    const auto expectCsr32Count = static_cast<int>(expect.GetNodeSize(NodeType::Csr32) / sizeof(Csr32Node));
    const auto actualCsr32Count = static_cast<int>(actual.GetNodeSize(NodeType::Csr32) / sizeof(Csr32Node));
    if (m_CmpCsr && !AreCsr32NodesMatched(pExpectCsr32, expectCsr32Count, pActualCsr32, actualCsr32Count))
    {
        PrintCsr32Diff(pExpectCsr32, expectCsr32Count, pActualCsr32, actualCsr32Count);
    }

    const auto pExpectMemory = expect.GetMemoryNode();
    const auto pActualMemory = actual.GetMemoryNode();
    const auto expectMemorySize = static_cast<size_t>(expect.GetNodeSize(NodeType::Memory));
    const auto actualMemorySize = static_cast<size_t>(actual.GetNodeSize(NodeType::Memory));
    if (m_CmpMemory && !IsMemoryNodeMatched(pExpectMemory, expectMemorySize, pActualMemory, actualMemorySize))
    {
        PrintMemoryDiff(pExpectMemory, expectMemorySize, pActualMemory, actualMemorySize);
    }
}
