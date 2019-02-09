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

namespace rafi { namespace fp {

uint32_t Add(uint32_t x, uint32_t y);
uint32_t Sub(uint32_t x, uint32_t y);
uint32_t Mul(uint32_t x, uint32_t y);
uint32_t Div(uint32_t x, uint32_t y);
uint32_t Sqrt(uint32_t x);
int Eq(uint32_t x, uint32_t y);
int Le(uint32_t x, uint32_t y);
int Lt(uint32_t x, uint32_t y);
uint32_t MulAdd(uint32_t x, uint32_t y, uint32_t z);
uint32_t MulSub(uint32_t x, uint32_t y, uint32_t z);
uint32_t NegMulAdd(uint32_t x, uint32_t y, uint32_t z);
uint32_t NegMulSub(uint32_t x, uint32_t y, uint32_t z);

int32_t FloatToInt32(uint32_t x);
uint32_t FloatToUInt32(uint32_t x);
uint32_t Int32ToFloat(int32_t x);
uint32_t UInt32ToFloat(uint32_t x);

uint32_t ConvertToRvFpClass(uint32_t x);

int GetRvExceptionFlags();
void SetRvExceptionFlags(int flags);

int GetRvRoundMode();
void SetRvRoundMode(int mode);

}}
