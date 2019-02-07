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

float Add(float x, float y);
float Sub(float x, float y);
float Mul(float x, float y);
float Div(float x, float y);
float Sqrt(float x);
int Eq(float x, float y);
int Le(float x, float y);
int Lt(float x, float y);

int32_t ConvertToInt32(float x);
uint32_t ConvertToUInt32(float x);
float ConvertToFloat(int32_t x);
float ConvertToFloat(uint32_t x);

uint32_t ConvertToRvFpClass(uint32_t rawValue);

int GetRvExceptionFlags();
void SetRvExceptionFlags(int flags);

int GetRvRoundMode();
void SetRvRoundMode(int mode);

}}
