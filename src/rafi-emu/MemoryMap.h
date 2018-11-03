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

#include <utility>

#include <rafi/BasicTypes.h>

namespace rafi { namespace emu {

// Memory Map
const PhysicalAddress RomAddr = 0x00001000;
const PhysicalAddress RamAddr = 0x80000000;
const PhysicalAddress UartAddr = 0x40002000;
const PhysicalAddress TimerAddr = 0x40000000;
const PhysicalAddress HostIoAddr = 0x80001000;

}}
