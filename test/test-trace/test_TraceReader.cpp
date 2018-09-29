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

#include <rvtrace/reader.h>
#include <rvtrace/writer.h>

#include "test_TraceUtil.h"

TEST(TraceReaderTest, Basic)
{
    char buffer[1024];

    auto builder = MakeTestBuilder();

    MemoryTraceWriter writer(buffer, sizeof(buffer));

    // cycle 0
    {
        auto prev = 0;
        auto next = builder->GetDataSize();

        builder->SetOffsetOfPreviousCycle(prev);
        builder->SetOffsetOfNextCycle(next);
        
        writer.Write(builder->GetData(), builder->GetDataSize());

    }

    // cycle 1
    {
        auto prev = -writer.GetPreviousWriteSize();
        auto next = 0;

        builder->SetOffsetOfPreviousCycle(prev);
        builder->SetOffsetOfNextCycle(next);

        writer.Write(builder->GetData(), builder->GetDataSize());
    }

    MemoryTraceReader reader(buffer, sizeof(buffer));

    // cycle 0
    ASSERT_TRUE(reader.IsFirstCycle());
    ASSERT_FALSE(reader.IsLastCycle());
    ASSERT_EQ(builder->GetDataSize(), reader.GetCurrentCycleDataSize());

    reader.MoveNextCycle();

    // cycle 1
    ASSERT_FALSE(reader.IsFirstCycle());
    ASSERT_TRUE(reader.IsLastCycle());
    ASSERT_EQ(builder->GetDataSize(), reader.GetCurrentCycleDataSize());

    reader.MovePreviousCycle();

    // cycle 0
    ASSERT_TRUE(reader.IsFirstCycle());
    ASSERT_FALSE(reader.IsLastCycle());
    ASSERT_EQ(builder->GetDataSize(), reader.GetCurrentCycleDataSize());
}

TEST(TraceReaderTest, OutOfRangeAccess)
{
    char buffer[1024];

    auto builder = MakeTestBuilder();

    MemoryTraceWriter writer(buffer, sizeof(buffer));

    {
        builder->SetOffsetOfPreviousCycle(0);
        builder->SetOffsetOfNextCycle(0);

        writer.Write(builder->GetData(), builder->GetDataSize());
    }

    MemoryTraceReader reader(buffer, sizeof(buffer));

    ASSERT_THROW(reader.MovePreviousCycle(), TraceException);
    ASSERT_THROW(reader.MoveNextCycle(), TraceException);
}
