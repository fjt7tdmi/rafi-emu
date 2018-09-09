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

#include "TraceCycleReader.h"

TraceCycleReader::TraceCycleReader(void* buffer, int64_t bufferSize)
{
}

int64_t TraceCycleReader::GetOffsetOfPreviousCycle()
{
    return 0;
}

int64_t TraceCycleReader::GetOffsetOfNextCycle()
{
    return 0;
}

const void* TraceCycleReader::GetNode(NodeType nodeType)
{
    return nullptr;
}

int64_t TraceCycleReader::GetNodeSize(NodeType nodeType)
{
    return 0;
}

bool TraceCycleReader::IsNodeExist(NodeType nodeType)
{
    return false;
}
