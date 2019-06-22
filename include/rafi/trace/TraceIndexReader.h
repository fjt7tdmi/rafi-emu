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

#include <rafi/common.h>

#include "ITraceReader.h"
#include "TraceBinaryMemoryReader.h"

namespace rafi { namespace trace {

class TraceIndexReaderImpl;

class TraceIndexReader : public ITraceReader
{
public:
    TraceIndexReader(const char* path);
    virtual ~TraceIndexReader();

    virtual const ICycle* GetCycle() const;

    virtual bool IsEnd() const;

    virtual void Next();
    virtual void Next(uint32_t cycle);

private:
    TraceIndexReaderImpl* m_pImpl;
};

}}
