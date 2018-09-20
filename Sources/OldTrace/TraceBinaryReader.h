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

#include <cstdint>
#include <cstdio>

#include <iostream>

#include "TraceBinary.h"
#include "../Trace/TraceException.h"

class TraceBinaryReader final
{
    TraceBinaryReader(const TraceBinaryReader&) = delete;
    TraceBinaryReader(TraceBinaryReader&&) = delete;
    TraceBinaryReader& operator=(const TraceBinaryReader&) = delete;
    TraceBinaryReader& operator=(TraceBinaryReader&&) = delete;

public:
    explicit TraceBinaryReader(const char* path);

    ~TraceBinaryReader();

    char* GetNode();

    // get node size which is pointed by internal cursor
    size_t GetNodeSize() const;

    // check if internal cursor point to the end of file
    bool IsEndNode() const;

    // move internal cursor to next cycle
    void MoveToNext();

    // move internal cursor to first cycle
    void MoveToFirst();

    // move internal cursor to last cycle
    void MoveToLast();

private:
    char* m_Buffer;

    // internal cursor
    char* m_Cursor;
};
