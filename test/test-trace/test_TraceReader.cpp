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

#include <memory>
#include <cstdlib>

#pragma warning(push)
#pragma warning(disable : 4389)
#include <gtest/gtest.h>
#pragma warning(pop)

#include <rafi/common.h>
#include <rafi/trace.h>

#include "test_TraceUtil.h"

using namespace rafi;
using namespace rafi::trace;

TEST(TraceReaderTest, Basic)
{
    char buffer[1024];

    auto builder = MakeTestBuilder();

    MemoryTraceWriter writer(buffer, sizeof(buffer));

    // cycle 0
    writer.Write(builder->GetData(), builder->GetDataSize());

    // cycle 1
    writer.Write(builder->GetData(), builder->GetDataSize());

    MemoryTraceReader reader(buffer, builder->GetDataSize() * 2);

    // cycle 0
    ASSERT_TRUE(reader.IsBegin());
    ASSERT_FALSE(reader.IsEnd());
    ASSERT_EQ(builder->GetDataSize(), reader.GetCurrentCycleDataSize());

    reader.MoveToNextCycle();

    // cycle 1
    ASSERT_FALSE(reader.IsBegin());
    ASSERT_FALSE(reader.IsEnd());
    ASSERT_EQ(builder->GetDataSize(), reader.GetCurrentCycleDataSize());

    reader.MoveToNextCycle();

    // end
    ASSERT_FALSE(reader.IsBegin());
    ASSERT_TRUE(reader.IsEnd());
}

TEST(TraceReaderTest, OutOfRangeAccess)
{
    char buffer[1024];

    auto builder = MakeTestBuilder();

    MemoryTraceWriter writer(buffer, sizeof(buffer));

    {
        writer.Write(builder->GetData(), builder->GetDataSize());
    }

    MemoryTraceReader reader(buffer, builder->GetDataSize());

    reader.MoveToNextCycle();
    ASSERT_THROW(reader.MoveToNextCycle(), TraceException);
}
