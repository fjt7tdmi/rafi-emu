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

#include <gtest/gtest.h>

#include <rvtrace/reader.h>
#include <rvtrace/writer.h>

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
    TraceCycleBuilder builder0(0);
    TraceCycleBuilder builder1(NodeFlag_BasicInfo);
    TraceCycleBuilder builder2(NodeFlag_BasicInfo | NodeFlag_Pc32);

    ASSERT_NE(nullptr, builder0.GetData());
    ASSERT_NE(nullptr, builder1.GetData());
    ASSERT_NE(nullptr, builder2.GetData());
}

TEST(TraceCycleTest, BuilderGetDataSize)
{
    TraceCycleBuilder builder0(0);
    TraceCycleBuilder builder1(NodeFlag_BasicInfo);
    TraceCycleBuilder builder2(NodeFlag_BasicInfo | NodeFlag_Pc32);

    ASSERT_EQ(sizeof(TraceCycleHeader), builder0.GetDataSize());
    ASSERT_EQ(sizeof(TraceCycleHeader) + sizeof(TraceCycleMetaNode) + sizeof(BasicInfoNode), builder1.GetDataSize());
    ASSERT_EQ(sizeof(TraceCycleHeader) + sizeof(TraceCycleMetaNode) * 2 + sizeof(BasicInfoNode) + sizeof(Pc32Node), builder2.GetDataSize());
}

TEST(TraceCycleTest, BuilderSetOffsetAndReaderGetOffset)
{
    // <previous, next> offsets
    const std::pair<int64_t, int64_t> offsetSets[] =
    {
        {   0, 0 },
        {  -8, 0 },
        {   0, 8 },
        { -24, 16 },
    };

    for (auto& offsetSet: offsetSets)
    {
        const auto previousOffset = offsetSet.first;
        const auto nextOffset = offsetSet.second;

        TraceCycleBuilder builder(NodeFlag_BasicInfo);

        builder.SetOffsetOfPreviousCycle(previousOffset);
        builder.SetOffsetOfNextCycle(nextOffset);

        TraceCycleReader reader(builder.GetData(), builder.GetDataSize());

        ASSERT_EQ(previousOffset, reader.GetOffsetOfPreviousCycle());
        ASSERT_EQ(nextOffset, reader.GetOffsetOfNextCycle());
    }
}

TEST(TraceCycleTest, BuilderSetNodeAndReaderGetNode)
{
    TraceCycleBuilder builder(NodeFlag_BasicInfo | NodeFlag_Pc32);

    BasicInfoNode basicInfo;
    Pc32Node pc32;
    Pc64Node pc64;

    FillRandom(&basicInfo, sizeof(basicInfo));
    FillRandom(&pc32, sizeof(pc32));
    FillRandom(&pc64, sizeof(pc64));

    builder.SetNode(NodeType::BasicInfo, &basicInfo, sizeof(basicInfo));
    builder.SetNode(NodeType::Pc32, &pc32, sizeof(pc32));

    ASSERT_THROW(builder.SetNode(NodeType::Pc64, &pc64, sizeof(pc64)), TraceCycleException);

    TraceCycleReader reader(builder.GetData(), builder.GetDataSize());

    ASSERT_EQ(sizeof(basicInfo), reader.GetNodeSize(NodeType::BasicInfo));
    ASSERT_EQ(sizeof(pc32), reader.GetNodeSize(NodeType::Pc32));

    ASSERT_THROW(reader.GetNodeSize(NodeType::Pc64), TraceCycleException);

    ASSERT_EQ(0, std::memcmp(&basicInfo, reader.GetNode(NodeType::BasicInfo), sizeof(basicInfo)));
    ASSERT_EQ(0, std::memcmp(&pc32, reader.GetNode(NodeType::Pc32), sizeof(pc32)));

    ASSERT_THROW(reader.GetNode(NodeType::Pc64), TraceCycleException);
}

TEST(TraceCycleTest, ReaderIsNodeExist)
{
    TraceCycleBuilder builder0(0);
    TraceCycleBuilder builder1(NodeFlag_BasicInfo);
    TraceCycleBuilder builder2(NodeFlag_BasicInfo | NodeFlag_Pc32);

    TraceCycleReader reader0(builder0.GetData(), builder0.GetDataSize());
    TraceCycleReader reader1(builder1.GetData(), builder1.GetDataSize());
    TraceCycleReader reader2(builder2.GetData(), builder2.GetDataSize());

    ASSERT_FALSE(reader0.IsNodeExist(NodeType::BasicInfo));
    ASSERT_TRUE(reader1.IsNodeExist(NodeType::BasicInfo));
    ASSERT_TRUE(reader2.IsNodeExist(NodeType::BasicInfo));

    ASSERT_FALSE(reader0.IsNodeExist(NodeType::Pc32));
    ASSERT_FALSE(reader1.IsNodeExist(NodeType::Pc32));
    ASSERT_TRUE(reader2.IsNodeExist(NodeType::Pc32));

    ASSERT_FALSE(reader0.IsNodeExist(NodeType::Pc32));
    ASSERT_FALSE(reader1.IsNodeExist(NodeType::Pc64));
    ASSERT_FALSE(reader2.IsNodeExist(NodeType::Pc64));
}

TEST(TraceCycleTest, DISABLED_BuilderConfig)
{
    FAIL();
}