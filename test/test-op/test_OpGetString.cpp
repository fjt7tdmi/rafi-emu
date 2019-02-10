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

#include <cstdlib>
#include <cstring>
#include <memory>

#include <gtest/gtest.h>

#include <rafi/common.h>

using namespace rafi::common;

TEST(OpTest, GetString)
{
    Decoder decoder;

    const Op& auipc = Op{ OpClass::RV32I, OpCode::auipc, OperandI {10, 3} };
    const Op& lui = decoder.Decode(0xfffff8b7);

    char buffer[64];

    SNPrintOp(buffer, sizeof(buffer), auipc);
    printf("%s\n", buffer);

    SNPrintOp(buffer, sizeof(buffer), lui);
    printf("%s\n", buffer);
}

