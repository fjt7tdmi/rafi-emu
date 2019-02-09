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
#include <cstring>

#include <rvtrace/reader.h>
#include <rvtrace/writer.h>

using namespace rvtrace;

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

std::unique_ptr<TraceCycleBuilder> MakeTestBuilder()
{
    TraceCycleConfig config;

    config.SetNodeCount(NodeType::BasicInfo, 1);

    auto builder = std::make_unique<TraceCycleBuilder>(config);

    BasicInfoNode basicInfo;

    FillRandom(&basicInfo, sizeof(basicInfo));

    builder->SetNode(NodeType::BasicInfo, 0, &basicInfo, sizeof(basicInfo));

    return std::move(builder);
}