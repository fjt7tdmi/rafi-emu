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

#include <cstdlib>
#include <cstring>
#include <memory>

#pragma warning(push)
#pragma warning(disable : 4389)
#include <gtest/gtest.h>
#pragma warning(pop)

#include <rafi/common.h>
#include <rafi/trace.h>

using namespace rafi;
using namespace rafi::trace;

namespace {
    void FillRandom(void* buffer, size_t size)
    {
        auto p = reinterpret_cast<uint8_t*>(buffer);

        for (size_t i = 0; i < size; i++)
        {
            p[i] = static_cast<uint8_t>(std::rand());
        }
    }

    void FillZero(void* buffer, size_t size)
    {
        std::memset(buffer, 0, size);
    }
}

TEST(TraceCycleTest, BuilderGetData)
{
    CycleConfig config0;
    CycleConfig config1;
    CycleConfig config2;

    config1.SetNodeCount(NodeType::BasicInfo, 1);
    config2.SetNodeCount(NodeType::BasicInfo, 1);
    config2.SetNodeCount(NodeType::Pc32, 1);

    CycleBuilder builder0(config0);
    CycleBuilder builder1(config1);
    CycleBuilder builder2(config2);

    ASSERT_NE(nullptr, builder0.GetData());
    ASSERT_NE(nullptr, builder1.GetData());
    ASSERT_NE(nullptr, builder2.GetData());
}

TEST(TraceCycleTest, BuilderGetDataSize)
{
    CycleConfig config0;
    CycleConfig config1;
    CycleConfig config2;

    config1.SetNodeCount(NodeType::BasicInfo, 1);
    config2.SetNodeCount(NodeType::BasicInfo, 1);
    config2.SetNodeCount(NodeType::Pc32, 1);

    CycleBuilder builder0(config0);
    CycleBuilder builder1(config1);
    CycleBuilder builder2(config2);

    ASSERT_EQ(sizeof(CycleHeader) + sizeof(CycleFooter), builder0.GetDataSize());
    ASSERT_EQ(sizeof(CycleHeader) + sizeof(CycleFooter) + sizeof(CycleMetaNode) + sizeof(BasicInfoNode), builder1.GetDataSize());
    ASSERT_EQ(sizeof(CycleHeader) + sizeof(CycleFooter) + sizeof(CycleMetaNode) * 2 + sizeof(BasicInfoNode) + sizeof(Pc32Node), builder2.GetDataSize());
}
