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

TEST(TraceCycleTest, BuilderSetNodeAndReaderGetNode)
{
    CycleConfig config;

    config.SetNodeCount(NodeType::BasicInfo, 1);
    config.SetNodeCount(NodeType::Pc32, 1);

    CycleBuilder builder(config);

    BasicInfoNode basicInfo;
    Pc32Node pc32;
    Pc64Node pc64;

    FillRandom(&basicInfo, sizeof(basicInfo));
    FillRandom(&pc32, sizeof(pc32));
    FillRandom(&pc64, sizeof(pc64));

    builder.SetNode(NodeType::BasicInfo, 0, &basicInfo, sizeof(basicInfo));
    builder.SetNode(NodeType::Pc32, 0, &pc32, sizeof(pc32));

    CycleView view(builder.GetData(), builder.GetDataSize());

    ASSERT_EQ(sizeof(basicInfo), view.GetNodeSize(NodeType::BasicInfo));
    ASSERT_EQ(sizeof(pc32), view.GetNodeSize(NodeType::Pc32));

    ASSERT_EQ(0, std::memcmp(&basicInfo, view.GetNode(NodeType::BasicInfo), sizeof(basicInfo)));
    ASSERT_EQ(0, std::memcmp(&pc32, view.GetNode(NodeType::Pc32), sizeof(pc32)));
}

TEST(TraceCycleTest, ReaderIsNodeExist)
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

    CycleView view0(builder0.GetData(), builder0.GetDataSize());
    CycleView view1(builder1.GetData(), builder1.GetDataSize());
    CycleView view2(builder2.GetData(), builder2.GetDataSize());

    ASSERT_EQ(0, view0.GetNodeCount(NodeType::BasicInfo));
    ASSERT_EQ(1, view1.GetNodeCount(NodeType::BasicInfo));
    ASSERT_EQ(1, view2.GetNodeCount(NodeType::BasicInfo));

    ASSERT_EQ(0, view0.GetNodeCount(NodeType::Pc32));
    ASSERT_EQ(0, view1.GetNodeCount(NodeType::Pc32));
    ASSERT_EQ(1, view2.GetNodeCount(NodeType::Pc32));

    ASSERT_EQ(0, view0.GetNodeCount(NodeType::Pc32));
    ASSERT_EQ(0, view1.GetNodeCount(NodeType::Pc64));
    ASSERT_EQ(0, view2.GetNodeCount(NodeType::Pc64));
}

TEST(TraceCycleTest, DISABLED_BuilderConfig)
{
    FAIL();
}