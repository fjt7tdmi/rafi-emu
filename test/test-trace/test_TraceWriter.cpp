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

using namespace rvtrace;

TEST(TraceWriterTest, Overflow)
{
    char buffer[4];

    auto builder = MakeTestBuilder();

    MemoryTraceWriter writer(buffer, sizeof(buffer));

    ASSERT_THROW(writer.Write(builder->GetData(), builder->GetDataSize()), TraceException);
}
