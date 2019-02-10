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

namespace rafi {

class DecoderImpl
{
public:
    Op Decode(uint32_t insn) const;

private:
    Op DecodeRV32I(uint32_t insn) const;
    Op DecodeRV32M(uint32_t insn) const;
    Op DecodeRV32A(uint32_t insn) const;
    Op DecodeRV32F(uint32_t insn) const;
    Op DecodeRV32D(uint32_t insn) const;

    Operand DecodeOperandR(uint32_t insn) const;
    Operand DecodeOperandR4(uint32_t insn) const;
    Operand DecodeOperandI(uint32_t insn) const;
    Operand DecodeOperandS(uint32_t insn) const;
    Operand DecodeOperandB(uint32_t insn) const;
    Operand DecodeOperandU(uint32_t insn) const;
    Operand DecodeOperandJ(uint32_t insn) const;
    Operand DecodeOperandShiftImm(uint32_t insn) const;
    Operand DecodeOperandCsr(uint32_t insn) const;
    Operand DecodeOperandCsrImm(uint32_t insn) const;
    Operand DecodeOperandFence(uint32_t insn) const;
};

}
