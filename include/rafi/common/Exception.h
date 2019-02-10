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

#include <iostream>

namespace rafi {

class FileOpenFailureException
{
public:
    explicit FileOpenFailureException(const char* path)
        : m_Path(path)
        , m_Message("")
    {
    }

    FileOpenFailureException(const char* path, const char* message)
        : m_Path(path)
        , m_Message(message)
    {
    }

    virtual void PrintMessage() const
    {
        std::cout << "[FileOpenFailureException] " << m_Path << " " << m_Message << std::endl;
    }

private:
    const char* m_Path;
    const char* m_Message;
};

}
