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

#include <gtest/gtest.h>

#include <rafi/common.h>
#include <rafi/trace.h>

#include "test_TraceUtil.h"

using namespace rafi::common;
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

    reader.MoveToPreviousCycle();

    // cycle 1
    ASSERT_FALSE(reader.IsBegin());
    ASSERT_FALSE(reader.IsEnd());
    ASSERT_EQ(builder->GetDataSize(), reader.GetCurrentCycleDataSize());

    reader.MoveToPreviousCycle();

    // cycle 0
    ASSERT_TRUE(reader.IsBegin());
    ASSERT_FALSE(reader.IsEnd());
    ASSERT_EQ(builder->GetDataSize(), reader.GetCurrentCycleDataSize());
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

    ASSERT_THROW(reader.MoveToPreviousCycle(), TraceException);

    reader.MoveToNextCycle();
    ASSERT_THROW(reader.MoveToNextCycle(), TraceException);
}
