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
#include <string>
#include <iostream>

#include "BasicTypes.h"

#define CHECK_RANGE(_lowerBound, _actual, _upperBound) \
    do { \
        if ((_actual) < (_lowerBound) || (_upperBound) < (_actual))  \
        { \
            std::cout << std::hex << "[IndexOutOfRange] lower: 0x" << _lowerBound << ", actual:0x" << _actual << ", upper:0x" << _upperBound << std::endl; \
            throw FatalException(__FILE__, __LINE__); \
        } \
    } while(0)

namespace rafi { namespace emu {

class NotImplementedException
{
public:
    NotImplementedException(const char *path, int line)
        : m_Path(path)
        , m_Line(line)
    {
    }

    virtual void PrintMessage() const
    {
        std::cout << "[NotImplementedException] " << m_Path << ":" << m_Line << std::endl;
    }

private:
    const char* m_Path;
    int m_Line;
};

class FatalException
{
public:
    FatalException(const char* path, int line)
        : m_Path(path)
        , m_Line(line)
    {
    }

    virtual void PrintMessage() const
    {
        std::cout << "[FatalException] " << m_Path << ":" << std::dec << m_Line << std::endl;
    }

private:
    const char* m_Path;
    int m_Line;
};

class InvalidAccessException
{
public:
    explicit InvalidAccessException(PhysicalAddress address)
        : m_Address(address)
    {
    }

    virtual void PrintMessage() const
    {
        std::cout << "[InvalidAccessException] " << std::hex << m_Address << std::endl;
    }

private:
    PhysicalAddress m_Address;
};

}}
