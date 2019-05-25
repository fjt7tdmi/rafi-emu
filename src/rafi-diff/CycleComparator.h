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

#include <rafi/trace.h>

namespace rafi {

class CycleComparator final
{
public:
    CycleComparator(bool cmpPhysicalPc)
        : m_CmpPhysicalPc(cmpPhysicalPc)
    {
    }

    // compare
    bool IsPcMatched(const trace::ICycle* expect, const trace::ICycle* actual) const;
    bool IsIntRegMatched(const trace::ICycle* expect, const trace::ICycle* actual) const;

    bool IsMatched(const trace::ICycle* expect, const trace::ICycle* actual) const;

    // print diff
    void PrintDiffPc(const trace::ICycle* expect, const trace::ICycle* actual) const;
    void PrintDiffIntReg(const trace::ICycle* expect, const trace::ICycle* actual) const;

    void PrintDiff(const trace::ICycle* expect, const trace::ICycle* actual) const;

private:
    bool m_CmpPhysicalPc;
};

}
