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
uint64_t Add(uint64_t x, uint64_t y);

uint32_t Sub(uint32_t x, uint32_t y);
uint64_t Sub(uint64_t x, uint64_t y);

uint32_t Mul(uint32_t x, uint32_t y);
uint64_t Mul(uint64_t x, uint64_t y);

uint32_t Div(uint32_t x, uint32_t y);
uint64_t Div(uint64_t x, uint64_t y);

uint32_t Sqrt(uint32_t x);
uint64_t Sqrt(uint64_t x);

bool Eq(uint32_t x, uint32_t y);
bool Eq(uint64_t x, uint64_t y);

bool Le(uint32_t x, uint32_t y);
bool Le(uint64_t x, uint64_t y);

bool Lt(uint32_t x, uint32_t y);
bool Lt(uint64_t x, uint64_t y);

uint32_t Min(uint32_t x, uint32_t y);
uint64_t Min(uint64_t x, uint64_t y);

uint32_t Max(uint32_t x, uint32_t y);
uint64_t Max(uint64_t x, uint64_t y);

uint32_t MulAdd(uint32_t x, uint32_t y, uint32_t z);
uint64_t MulAdd(uint64_t x, uint64_t y, uint64_t z);

uint32_t MulSub(uint32_t x, uint32_t y, uint32_t z);
uint64_t MulSub(uint64_t x, uint64_t y, uint64_t z);

uint32_t NegMulAdd(uint32_t x, uint32_t y, uint32_t z);
uint64_t NegMulAdd(uint64_t x, uint64_t y, uint64_t z);

uint32_t NegMulSub(uint32_t x, uint32_t y, uint32_t z);
uint64_t NegMulSub(uint64_t x, uint64_t y, uint64_t z);

int32_t DoubleToInt32(uint64_t x, int roundMode);
uint32_t DoubleToUInt32(uint64_t x, int roundMode);

int64_t DoubleToInt64(uint64_t x, int roundMode);
uint64_t DoubleToUInt64(uint64_t x, int roundMode);

int32_t FloatToInt32(uint32_t x, int roundMode);
uint32_t FloatToUInt32(uint32_t x, int roundMode);

uint64_t Int32ToDouble(int32_t x);
uint64_t UInt32ToDouble(uint32_t x);

uint32_t Int32ToFloat(int32_t x);
uint32_t UInt32ToFloat(uint32_t x);

uint32_t DoubleToFloat(uint64_t x);
uint64_t FloatToDouble(uint32_t x);

uint32_t ConvertToRvFpClass(uint32_t x);
uint32_t ConvertToRvFpClass(uint64_t x);

int GetRvExceptionFlags();
void SetRvExceptionFlags(int flags);

int GetRvRoundMode();
void SetRvRoundMode(int mode);

}}
