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

#include "TraceBinary.h"
#include "TraceBinaryComparator.h"
#include "TraceBinaryUtil.h"
#include "TraceException.h"

bool TraceBinaryComparator::IsPc32NodeMatched(Pc32Node* pExpect, Pc32Node* pActual) const
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

bool TraceBinaryComparator::IsIntReg32NodeMatched(IntReg32Node* pExpect, IntReg32Node* pActual) const
{
    if (pExpect == nullptr || pActual == nullptr)
    {
        return false;
    }

    return std::memcmp(pExpect->regs, pActual->regs, sizeof(pExpect->regs)) == 0;
}

bool TraceBinaryComparator::IsCsr32NodeMatched(Csr32NodeHeader* pExpect, Csr32NodeHeader* pActual) const
{
    if (pExpect == nullptr || pActual == nullptr)
    {
        return false;
    }

    if (pExpect->bodySize != pActual->bodySize)
    {
        return false;
    }

    if (pExpect->bodySize < 0)
    {
        throw TraceException("Detect minus node size.");
    }

#if INT64_MAX > SIZE_MAX
    if (pExpect->bodySize > SIZE_MAX)
    {
        throw TraceException("Detect too big node size.");
    }
#endif

    const size_t bodySize = static_cast<size_t>(pExpect->bodySize);
    const void* expectBody = &pExpect[1];
    const void* actualBody = &pActual[1];

    return std::memcmp(expectBody, actualBody, bodySize) == 0;
}

bool TraceBinaryComparator::IsMemoryNodeMatched(MemoryNodeHeader* pExpect, MemoryNodeHeader* pActual) const
{
    if (pExpect == nullptr || pActual == nullptr)
    {
        return false;
    }

    if (pExpect->bodySize != pActual->bodySize)
    {
        return false;
    }

    if (pExpect->bodySize < 0)
    {
        throw TraceException("Detect minus node size.");
    }

#if INT64_MAX > SIZE_MAX
    if (pExpect->bodySize > SIZE_MAX)
    {
        throw TraceException("Detect too big node size.");
    }
#endif

    const size_t bodySize = static_cast<size_t>(pExpect->bodySize);
    const void* expectBody = &pExpect[1];
    const void* actualBody = &pActual[1];

    return std::memcmp(expectBody, actualBody, bodySize) == 0;
}

bool TraceBinaryComparator::AreTraceChildsMatched(char* pExpect, size_t expectSize, char* pActual, size_t actualSize) const
{
    if (!IsPc32NodeMatched(FindPc32Node(pExpect, expectSize), FindPc32Node(pActual, actualSize)))
    {
        return false;
    }
    if (!IsIntReg32NodeMatched(FindIntReg32Node(pExpect, expectSize), FindIntReg32Node(pActual, actualSize)))
    {
        return false;
    }
    if (m_CmpCsr && !IsCsr32NodeMatched(FindCsr32Node(pExpect, expectSize), FindCsr32Node(pActual, actualSize)))
    {
        return false;
    }
    if (m_CmpMemory && !IsMemoryNodeMatched(FindMemoryNode(pExpect, expectSize), FindMemoryNode(pActual, actualSize)))
    {
        return false;
    }
    return true;
}

void TraceBinaryComparator::PrintPc32Diff(Pc32Node* pExpect, Pc32Node* pActual) const
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

void TraceBinaryComparator::PrintIntReg32Diff(IntReg32Node* pExpect, IntReg32Node* pActual) const
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

void TraceBinaryComparator::PrintCsr32Diff(Csr32NodeHeader* pExpect, Csr32NodeHeader* pActual) const
{
    if (pExpect == nullptr || pActual == nullptr)
    {
        if (pExpect == nullptr)
        {
            printf("    - expect has no Csr32Node.\n");
        }
        if (pActual == nullptr)
        {
            printf("    - actual has no Csr32Node.\n");
        }
        return;
    }

    if (pExpect->bodySize != pActual->bodySize)
    {
        printf("    - Csr32Node body size is not matched (expect:0x%016llx, actual:0x%016I64x).\n", pExpect->bodySize, pActual->bodySize);
        return;
    }

    const int unitSize = sizeof(uint32_t);
    if (pExpect->bodySize % unitSize != 0)
    {
        printf("    - Csr32Node body size must be multiple of %d. Cannot print!\n", unitSize);
        return;
    }

    const auto count = pExpect->bodySize / unitSize;

    const auto expectBody = reinterpret_cast<const uint32_t*>(&pExpect[1]);
    const auto actualBody = reinterpret_cast<const uint32_t*>(&pActual[1]);

    for (int i = 0; i < count; i++)
    {
        if (expectBody[i] != actualBody[i])
        {
            printf("    - csr value is not matched (offset=0x%93x, name=%s).\n", i, GetString(static_cast<csr_addr_t>(i)));
            printf("      expect: 0x%08x\n", expectBody[i]);
            printf("      actual: 0x%08x\n", actualBody[i]);
        }
    }
}

void TraceBinaryComparator::PrintMemoryDiff(MemoryNodeHeader* pExpect, MemoryNodeHeader* pActual) const
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

    if (pExpect->bodySize != pActual->bodySize)
    {
        printf("    - MemoryNode body size is not matched (expect:0x%016I64x, actual:0x%016I64x).\n", pExpect->bodySize, pActual->bodySize);
        return;
    }

    const int64_t unitSize = 16;
    if (pExpect->bodySize % unitSize != 0)
    {
        throw TraceException("MemoryNode body size must be multiple of 16.");
    }

    const auto count = pExpect->bodySize / unitSize;

    const auto expectBody = reinterpret_cast<const uint8_t*>(&pExpect[1]);
    const auto actualBody = reinterpret_cast<const uint8_t*>(&pActual[1]);

    for (int64_t i = 0; i < count; i++)
    {
        const auto offset = i * unitSize;
        if (std::memcmp(&expectBody[offset], &actualBody[offset], unitSize) != 0)
        {
            printf("    - memory value is not matched (offset=0x%16I64x).\n", offset);
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

void TraceBinaryComparator::PrintDiff(char* pExpect, size_t expectSize, char* pActual, size_t actualSize) const
{
    auto pExpectPc32 = FindPc32Node(pExpect, expectSize);
    auto pActualPc32 = FindPc32Node(pActual, actualSize);
    if (!IsPc32NodeMatched(pExpectPc32, pActualPc32))
    {
        PrintPc32Diff(pExpectPc32, pActualPc32);
    }

    auto pExpectIntReg32 = FindIntReg32Node(pExpect, expectSize);
    auto pActualIntReg32 = FindIntReg32Node(pActual, actualSize);
    if (!IsIntReg32NodeMatched(pExpectIntReg32, pActualIntReg32))
    {
        PrintIntReg32Diff(pExpectIntReg32, pActualIntReg32);
    }

    auto pExpectCsr32 = FindCsr32Node(pExpect, expectSize);
    auto pActualCsr32 = FindCsr32Node(pActual, actualSize);
    if (m_CmpCsr && !IsCsr32NodeMatched(pExpectCsr32, pActualCsr32))
    {
        PrintCsr32Diff(pExpectCsr32, pActualCsr32);
    }

    auto pExpectMemory = FindMemoryNode(pExpect, expectSize);
    auto pActualMemory = FindMemoryNode(pActual, actualSize);
    if (m_CmpMemory && !IsMemoryNodeMatched(pExpectMemory, pActualMemory))
    {
        PrintMemoryDiff(pExpectMemory, pActualMemory);
    }
}
