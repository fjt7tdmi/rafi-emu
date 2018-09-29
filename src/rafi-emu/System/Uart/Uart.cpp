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

#include <cstdio>

#include "Uart.h"

int8_t Uart::GetInt8(int address) const
{
    (void)address;
    printf("UART read\n");
    return 0;
}

void Uart::SetInt8(int address, int8_t value)
{
    (void)address;
    printf("UART write (0x%02x)\n", value);
}

int16_t Uart::GetInt16(int address) const
{
    (void)address;
    printf("UART read\n");
    return 0;
}

void Uart::SetInt16(int address, int16_t value)
{
    (void)address;
    printf("UART write (0x%04x)\n", value);
}

int32_t Uart::GetInt32(int address) const
{
    (void)address;
    printf("UART read\n");
    return 0;
}

void Uart::SetInt32(int address, int32_t value)
{
    (void)address;
    printf("UART write (0x%08x)\n", value);
}
