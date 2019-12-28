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

#include <rafi/emu.h>

#include "CommandLineOption.h"
#include "System.h"
#include "TraceLogger.h"
#include "IEmulator.h"

namespace rafi { namespace emu {

class Emulator final : public IEmulator
{
public:
    Emulator(CommandLineOption option);
    virtual ~Emulator();

    void LoadFileToMemory(const char* path, paddr_t address);
    void PrintStatus() const;
    int GetCycle() const;

    void Process(EmulationStop condition, int cycle);
    void Process(EmulationStop condition) override;
    void ProcessCycle() override;

    bool IsValidMemory(paddr_t addr, size_t size) const override;
    void ReadMemory(void* pOutBuffer, size_t bufferSize, paddr_t addr) override;
    void WriteMemory(const void* pBuffer, size_t bufferSize, paddr_t addr) override;

    vaddr_t GetPc() const override;
    void CopyIntReg(trace::NodeIntReg32* pOut) const override;
    void CopyIntReg(trace::NodeIntReg64* pOut) const override;

private:
    static const int CycleForever = -1;

    bool IsStopConditionFilledPre(EmulationStop condition);
    bool IsStopConditionFilledPost(EmulationStop condition);

    const CommandLineOption& m_Option;
    System m_System;
    TraceLogger m_Logger;

    int m_Cycle{0};
};

}}
