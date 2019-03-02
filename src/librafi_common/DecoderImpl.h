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
    DecoderImpl(XLEN xlen);

    Op Decode(uint16_t insn) const;
    Op Decode(uint32_t insn) const;

    bool IsCompressedInstruction(uint16_t insn) const;
    bool IsCompressedInstruction(uint32_t insn) const;

private:
    XLEN m_XLEN;

    Op DecodeI(uint32_t insn) const;
    Op DecodeM(uint32_t insn) const;
    Op DecodeA(uint32_t insn) const;
    Op DecodeF(uint32_t insn) const;
    Op DecodeD(uint32_t insn) const;

    Op DecodeC(uint16_t insn) const;

    Operand DecodeOperandR(uint32_t insn) const;
    Operand DecodeOperandR4(uint32_t insn) const;
    Operand DecodeOperandI(uint32_t insn) const;
    Operand DecodeOperandS(uint32_t insn) const;
    Operand DecodeOperandB(uint32_t insn) const;
    Operand DecodeOperandU(uint32_t insn) const;
    Operand DecodeOperandJ(uint32_t insn) const;
    Operand DecodeOperandShiftImm_32(uint32_t insn) const;
    Operand DecodeOperandShiftImm_64(uint32_t insn) const;
    Operand DecodeOperandCsr(uint32_t insn) const;
    Operand DecodeOperandCsrImm(uint32_t insn) const;
    Operand DecodeOperandFence(uint32_t insn) const;

    Operand DecodeOperandCR(uint16_t insn) const;
    Operand DecodeOperandCR_Alu(uint16_t insn) const;
    Operand DecodeOperandCI(uint16_t insn, bool immSigned) const;
    Operand DecodeOperandCI_ADDI16SP(uint16_t insn) const;
    Operand DecodeOperandCI_AluImm(uint16_t insn, bool immSigned) const;
    Operand DecodeOperandCI_LoadSP(uint16_t insn, int accessSize) const;
    Operand DecodeOperandCI_LUI(uint16_t insn) const;
    Operand DecodeOperandCSS(uint16_t insn, int accessSize) const;
    Operand DecodeOperandCIW(uint16_t insn) const;
    Operand DecodeOperandCL(uint16_t insn, int accessSize) const;
    Operand DecodeOperandCS(uint16_t insn, int accessSize) const;
    Operand DecodeOperandCB(uint16_t insn) const;
    Operand DecodeOperandCJ(uint16_t insn) const;
};

}
