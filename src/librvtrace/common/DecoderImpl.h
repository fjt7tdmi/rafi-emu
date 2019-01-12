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

#include <rvtrace/common.h>

namespace rvtrace {

class DecoderImpl
{
public:
    Op Decode(int32_t insn) const;

private:
    Op DecodeRV32I(int32_t insn) const;
    Op DecodeRV32M(int32_t insn) const;
    Op DecodeRV32A(int32_t insn) const;

    Operand DecodeOperandR(int32_t insn) const;
    Operand DecodeOperandR4(int32_t insn) const;
    Operand DecodeOperandI(int32_t insn) const;
    Operand DecodeOperandS(int32_t insn) const;
    Operand DecodeOperandB(int32_t insn) const;
    Operand DecodeOperandU(int32_t insn) const;
    Operand DecodeOperandJ(int32_t insn) const;
    Operand DecodeOperandShiftImm(int32_t insn) const;
    Operand DecodeOperandCsr(int32_t insn) const;
    Operand DecodeOperandCsrImm(int32_t insn) const;
    Operand DecodeOperandFence(int32_t insn) const;
};

}
