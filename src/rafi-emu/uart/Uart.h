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

#include <vector>

#include <emu/BasicTypes.h>

#include "../bus/IBusSlave.h"

#include "UartTypes.h"

class Uart : public IBusSlave
{
public:
    virtual int8_t GetInt8(int address) override;
    virtual void SetInt8(int address, int8_t value) override;

    virtual int16_t GetInt16(int address) override;
    virtual void SetInt16(int address, int16_t value) override;

    virtual int32_t GetInt32(int address) override;
    virtual void SetInt32(int address, int32_t value) override;

    virtual int GetSize() const override
    {
        return RegSize;
    }

    void ProcessCycle();

private:
    static const int RegSize = 32;
    static const int InitialRxCycle = 100;
    static const int RxCycle = 50;

    int32_t Read(int address, int size);
    void Write(int address, int32_t value, int size);

    void UpdateRx();
    void PrintTx();

    InterruptEnable m_InterruptEnable;
    InterruptPending m_InterruptPending;

    std::vector<char> m_TxChars;
    char m_RxChar {'\0'};

    int m_Cycle {0};
    size_t m_PrintCount {0};
};