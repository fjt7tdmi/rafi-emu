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

#include <string>
#include <vector>

#include <rafi/emu.h>

namespace rafi { namespace emu {

class LoadOption
{
public:
    explicit LoadOption(const std::string& arg);

    const std::string& GetPath() const;

    uint64_t GetAddress() const;

private:
    std::string m_Path;
    uint64_t m_Address;
};

class CommandLineOption
{
public:
    CommandLineOption(int argc, char** argv);

    bool IsDumpEnabled() const;
    bool IsDumpCsrEnabled() const;
    bool IsDumpMemoryEnabled() const;
    bool IsHostIoEnabled() const;

    const std::string& GetDumpPath() const;
    const std::vector<LoadOption>& GetLoadOptions() const;
    XLEN GetXLEN() const;

    int GetCycle() const;
    int GetDumpSkipCycle() const;

    size_t GetRamSize() const;

    uint64_t GetHostIoAddress() const;
    uint64_t GetPc() const;

private:
    static const int DefaultRamSize = 64 * 1024 * 1024;

    uint64_t ParseHex(const std::string str);

    std::string m_DumpPath;
    std::vector<LoadOption> m_LoadOptions;

    XLEN m_XLEN {XLEN::XLEN32};

    int m_Cycle {0};
    int m_DumpSkipCycle {0};

    size_t m_RamSize {0};

    uint64_t m_HostIoAddress {0};
    uint64_t m_Pc {0};

    bool m_DumpEnabled {false};
    bool m_DumpCsrEnabled {false};
    bool m_DumpMemoryEnabled {false};
    bool m_HostIoEnabled {false};
};

}}
