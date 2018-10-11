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

#include <emu/BitField.h>

#include "../Bus/IBusSlave.h"

enum Address
{
    Address_TxData = 0,
    Address_RxData = 4,
    Address_InterruptEnable = 16,
    Address_InterruptPending = 24,
};

struct InterruptEnable : BitField
{
    InterruptEnable() : BitField(0)
    {
    }

    InterruptEnable(int32_t value) : BitField(value)
    {
    }

    using TXIE = BitFieldMember<1>;
    using RXIE = BitFieldMember<2>;
};

struct InterruptPending : BitField
{
    InterruptPending() : BitField(0)
    {
    }

    InterruptPending(int32_t value) : BitField(value)
    {
    }

    using TXIP = BitFieldMember<1>;
    using RXIP = BitFieldMember<2>;
};
