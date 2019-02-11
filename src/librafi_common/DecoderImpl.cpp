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

#include <cassert>
#include <cstdio>
#include <cstring>
#include <variant>

#include "DecoderImpl.h"

namespace rafi {

namespace {

inline int32_t sext(int32_t value, int msb)
{
    if ((value >> msb) & 0x1)
    {
        // minus
        const auto mask = static_cast<int32_t>(-1) << msb;
        return value | mask;
    }
    else
    {
        // plus
        return value;
    }
}

inline int32_t SignExtend(int srcWidth, int32_t srcValue)
{
    assert(srcWidth > 0);
    return sext(srcValue, srcWidth - 1);
}

inline int32_t ZeroExtend(int srcWidth, int32_t srcValue)
{
    assert(srcWidth > 0);

    const int32_t mask = (1 << srcWidth) - 1;

    return srcValue & mask;
}

inline int32_t Pick(uint32_t insn, int lsb, int width = 1)
{
    assert(0 <= lsb && lsb < 32);
    assert(1 <= width && width < 32);
    return (insn >> lsb) & ((1 << width) - 1);
}

}

Op DecoderImpl::Decode(uint16_t insn) const
{
    return DecodeRV32C(insn);
}

Op DecoderImpl::Decode(uint32_t insn) const
{
    const auto opcode = Pick(insn, 0, 7);
    const auto funct3 = Pick(insn, 12, 3);
    const auto funct7 = Pick(insn, 25, 7);
    const auto funct2 = Pick(insn, 25, 2);

    if (IsCompressedInstruction(insn))
    {
        return DecodeRV32C(static_cast<uint16_t>(insn));
    }
    else if (opcode == 0b0110011 && funct7 == 0b0000001)
    {
        return DecodeRV32M(insn);
    }
    else if (opcode == 0b0101111 && funct3 == 0b010)
    {
        return DecodeRV32A(insn);
    }
    else if ((opcode == 0b0000111 && funct3 == 0b010) ||
             (opcode == 0b0100111 && funct3 == 0b010) ||
             (opcode == 0b1000011 && funct2 == 0b00) ||
             (opcode == 0b1000111 && funct2 == 0b00) ||
             (opcode == 0b1001011 && funct2 == 0b00) ||
             (opcode == 0b1001111 && funct2 == 0b00) ||
             (opcode == 0b1010011 && funct2 == 0b00 && !(funct7 == 0b0100000)))
    {
        return DecodeRV32F(insn);
    }
    else if ((opcode == 0b0000111 && funct3 == 0b011) ||
             (opcode == 0b0100111 && funct3 == 0b011) ||
             (opcode == 0b1000011 && funct2 == 0b01) ||
             (opcode == 0b1000111 && funct2 == 0b01) ||
             (opcode == 0b1001011 && funct2 == 0b01) ||
             (opcode == 0b1001111 && funct2 == 0b01) ||
             (opcode == 0b1010011 && funct2 == 0b01) ||
             (opcode == 0b1010011 && funct7 == 0b0100000))
    {
        return DecodeRV32D(insn);
    }
    else
    {
        return DecodeRV32I(insn);
    }
}

bool DecoderImpl::IsCompressedInstruction(uint16_t insn) const
{
    return (insn & 0b11) != 0b11;
}

bool DecoderImpl::IsCompressedInstruction(uint32_t insn) const
{
    return (insn & 0b11) != 0b11;
}

Op DecoderImpl::DecodeRV32I(uint32_t insn) const
{
    const auto opcode = Pick(insn, 0, 7);
    const auto funct3 = Pick(insn, 12, 3);
    const auto funct7 = Pick(insn, 25, 7);
    const auto funct12 = Pick(insn, 20, 12);
    const auto rd = Pick(insn, 7, 5);
    const auto rs1 = Pick(insn, 15, 5);

    switch (opcode)
    {
    case 0b0110111:
        return Op{ OpClass::RV32I, OpCode::lui, DecodeOperandU(insn) };
    case 0b0010111:
        return Op{ OpClass::RV32I, OpCode::auipc, DecodeOperandU(insn) };
    case 0b1101111:
        return Op{ OpClass::RV32I, OpCode::jal, DecodeOperandJ(insn) };
    case 0b1100111:
        if (funct3 == 0)
        {
            return Op{ OpClass::RV32I, OpCode::jalr, DecodeOperandI(insn) };
        }
        else
        {
            return Op{ OpClass::RV32I, OpCode::unknown, OperandNone() };
        }
    case 0b1100011:
        if (funct3 == 0)
        {
            return Op{ OpClass::RV32I, OpCode::beq, DecodeOperandB(insn) };
        }
        else if (funct3 == 1)
        {
            return Op{ OpClass::RV32I, OpCode::bne, DecodeOperandB(insn) };
        }
        else if (funct3 == 4)
        {
            return Op{ OpClass::RV32I, OpCode::blt, DecodeOperandB(insn) };
        }
        else if (funct3 == 5)
        {
            return Op{ OpClass::RV32I, OpCode::bge, DecodeOperandB(insn) };
        }
        else if (funct3 == 6)
        {
            return Op{ OpClass::RV32I, OpCode::bltu, DecodeOperandB(insn) };
        }
        else if (funct3 == 7)
        {
            return Op{ OpClass::RV32I, OpCode::bgeu, DecodeOperandB(insn) };
        }
        else
        {
            return Op{ OpClass::RV32I, OpCode::unknown, OperandNone() };
        }
    case 0b0000011:
        if (funct3 == 0)
        {
            return Op{ OpClass::RV32I, OpCode::lb, DecodeOperandI(insn) };
        }
        else if (funct3 == 1)
        {
            return Op{ OpClass::RV32I, OpCode::lh, DecodeOperandI(insn) };
        }
        else if (funct3 == 2)
        {
            return Op{ OpClass::RV32I, OpCode::lw, DecodeOperandI(insn) };
        }
        else if (funct3 == 4)
        {
            return Op{ OpClass::RV32I, OpCode::lbu, DecodeOperandI(insn) };
        }
        else if (funct3 == 5)
        {
            return Op{ OpClass::RV32I, OpCode::lhu, DecodeOperandI(insn) };
        }
        else
        {
            return Op{ OpClass::RV32I, OpCode::unknown, OperandNone() };
        }
    case 0b0100011:
        if (funct3 == 0)
        {
            return Op{ OpClass::RV32I, OpCode::sb, DecodeOperandS(insn) };
        }
        else if (funct3 == 1)
        {
            return Op{ OpClass::RV32I, OpCode::sh, DecodeOperandS(insn) };
        }
        else if (funct3 == 2)
        {
            return Op{ OpClass::RV32I, OpCode::sw, DecodeOperandS(insn) };
        }
        else
        {
            return Op{ OpClass::RV32I, OpCode::unknown, OperandNone() };
        }
    case 0b0010011:
        if (funct3 == 0)
        {
            return Op{ OpClass::RV32I, OpCode::addi, DecodeOperandI(insn) };
        }
        else if (funct3 == 1 && funct7 == 0b0000000)
        {
            return Op{ OpClass::RV32I, OpCode::slli, DecodeOperandShiftImm(insn) };
        }
        else if (funct3 == 2)
        {
            return Op{ OpClass::RV32I, OpCode::slti, DecodeOperandI(insn) };
        }
        else if (funct3 == 3)
        {
            return Op{ OpClass::RV32I, OpCode::sltiu, DecodeOperandI(insn) };
        }
        else if (funct3 == 4)
        {
            return Op{ OpClass::RV32I, OpCode::xori, DecodeOperandI(insn) };
        }
        else if (funct3 == 5 && funct7 == 0b0000000)
        {
            return Op{ OpClass::RV32I, OpCode::srli, DecodeOperandShiftImm(insn) };
        }
        else if (funct3 == 5 && funct7 == 0b0100000)
        {
            return Op{ OpClass::RV32I, OpCode::srai, DecodeOperandShiftImm(insn) };
        }
        else if (funct3 == 6)
        {
            return Op{ OpClass::RV32I, OpCode::ori, DecodeOperandI(insn) };
        }
        else if (funct3 == 7)
        {
            return Op{ OpClass::RV32I, OpCode::andi, DecodeOperandI(insn) };
        }
        else
        {
            return Op{ OpClass::RV32I, OpCode::unknown, OperandNone() };
        }
    case 0b0110011:
        if (funct7 == 0b0000000)
        {
            // RV32I
            if (funct3 == 0)
            {
                return Op{ OpClass::RV32I, OpCode::add, DecodeOperandR(insn) };
            }
            else if (funct3 == 1)
            {
                return Op{ OpClass::RV32I, OpCode::sll, DecodeOperandR(insn) };
            }
            else if (funct3 == 2)
            {
                return Op{ OpClass::RV32I, OpCode::slt, DecodeOperandR(insn) };
            }
            else if (funct3 == 3)
            {
                return Op{ OpClass::RV32I, OpCode::sltu, DecodeOperandR(insn) };
            }
            else if (funct3 == 4)
            {
                return Op{ OpClass::RV32I, OpCode::xor_, DecodeOperandR(insn) };
            }
            else if (funct3 == 5)
            {
                return Op{ OpClass::RV32I, OpCode::srl, DecodeOperandR(insn) };
            }
            else if (funct3 == 6)
            {
                return Op{ OpClass::RV32I, OpCode::or_, DecodeOperandR(insn) };
            }
            else if (funct3 == 7)
            {
                return Op{ OpClass::RV32I, OpCode::and_, DecodeOperandR(insn) };
            }
            else
            {
                return Op{ OpClass::RV32I, OpCode::unknown, OperandNone() };
            }
        }
        else if (funct7 == 0b0100000)
        {
            if (funct3 == 0)
            {
                return Op{ OpClass::RV32I, OpCode::sub, DecodeOperandR(insn) };
            }
            else if (funct3 == 5)
            {
                return Op{ OpClass::RV32I, OpCode::sra, DecodeOperandR(insn) };
            }
            else
            {
                return Op{ OpClass::RV32I, OpCode::unknown, OperandNone() };
            }
        }
        else
        {
            return Op{ OpClass::RV32I, OpCode::unknown, OperandNone() };
        }
    case 0b0001111:
        if (funct3 == 0 && rd == 0 && rs1 == 0 && Pick(insn, 28, 4) == 0)
        {
            return Op{ OpClass::RV32I, OpCode::fence, DecodeOperandFence(insn) };
        }
        else if ( funct3 == 1 && rd == 0 && rs1 == 0 && funct12 == 0)
        {
            return Op{ OpClass::RV32I, OpCode::fence_i, DecodeOperandFence(insn) };
        }
        else
        {
            return Op{ OpClass::RV32I, OpCode::unknown, OperandNone() };
        }
    case 0b1110011:
        if (funct3 == 0 && rd == 0)
        {
            if (funct7 == 0b0001001)
            {
                return Op{ OpClass::RV32I, OpCode::sfence_vma, DecodeOperandR(insn) };
            }
            else if (rs1 == 0)
            {
                switch (funct12)
                {
                case 0b000000000000:
                    return Op{ OpClass::RV32I, OpCode::ecall, OperandNone() };
                case 0b000000000001:
                    return Op{ OpClass::RV32I, OpCode::ebreak, OperandNone() };
                case 0b000000000010:
                    return Op{ OpClass::RV32I, OpCode::uret, OperandNone() };
                case 0b000100000010:
                    return Op{ OpClass::RV32I, OpCode::sret, OperandNone() };
                case 0b000100000101:
                    return Op{ OpClass::RV32I, OpCode::wfi, OperandNone() };
                case 0b001100000010:
                    return Op{ OpClass::RV32I, OpCode::mret, OperandNone() };
                default:
                    return Op{ OpClass::RV32I, OpCode::unknown, OperandNone() };
                }
            }
            else
            {
                return Op{ OpClass::RV32I, OpCode::unknown, OperandNone() };
            }
        }
        else if (funct3 == 1)
        {
            return Op{ OpClass::RV32I, OpCode::csrrw, DecodeOperandCsr(insn) };
        }
        else if (funct3 == 2)
        {
            return Op{ OpClass::RV32I, OpCode::csrrs, DecodeOperandCsr(insn) };
        }
        else if (funct3 == 3)
        {
            return Op{ OpClass::RV32I, OpCode::csrrc, DecodeOperandCsr(insn) };
        }
        else if (funct3 == 5)
        {
            return Op{ OpClass::RV32I, OpCode::csrrwi, DecodeOperandCsrImm(insn) };
        }
        else if (funct3 == 6)
        {
            return Op{ OpClass::RV32I, OpCode::csrrsi, DecodeOperandCsrImm(insn) };
        }
        else if (funct3 == 7)
        {
            return Op{ OpClass::RV32I, OpCode::csrrci, DecodeOperandCsrImm(insn) };
        }
        else
        {
            return Op{ OpClass::RV32I, OpCode::unknown, OperandNone() };
        }
    default:
        return Op{ OpClass::RV32I, OpCode::unknown, OperandNone() };
    }
}

Op DecoderImpl::DecodeRV32M(uint32_t insn) const
{
    const auto funct3 = Pick(insn, 12, 3);

    switch (funct3)
    {
    case 0:
        return Op{ OpClass::RV32M, OpCode::mul, DecodeOperandR(insn) };
    case 1:
        return Op{ OpClass::RV32M, OpCode::mulh, DecodeOperandR(insn) };
    case 2:
        return Op{ OpClass::RV32M, OpCode::mulhsu, DecodeOperandR(insn) };
    case 3:
        return Op{ OpClass::RV32M, OpCode::mulhu, DecodeOperandR(insn) };
    case 4:
        return Op{ OpClass::RV32M, OpCode::div, DecodeOperandR(insn) };
    case 5:
        return Op{ OpClass::RV32M, OpCode::divu, DecodeOperandR(insn) };
    case 6:
        return Op{ OpClass::RV32M, OpCode::rem, DecodeOperandR(insn) };
    case 7:
        return Op{ OpClass::RV32M, OpCode::remu, DecodeOperandR(insn) };
    default:
        return Op{ OpClass::RV32M, OpCode::unknown, OperandNone() };
    }
}

Op DecoderImpl::DecodeRV32A(uint32_t insn) const
{
    const auto funct5 = Pick(insn, 27, 5);
    const auto rs2 = Pick(insn, 20, 5);

    switch (funct5)
    {
    case 0b00010:
        if (rs2 == 0b00000)
        {
            return Op{ OpClass::RV32A, OpCode::lr_w, DecodeOperandR(insn) };
        }
        else
        {
            return Op{ OpClass::RV32A, OpCode::unknown, OperandNone() };
        }
    case 0b00011:
        return Op{ OpClass::RV32A, OpCode::sc_w, DecodeOperandR(insn) };
    case 0b00001:
        return Op{ OpClass::RV32A, OpCode::amoswap_w, DecodeOperandR(insn) };
    case 0b00000:
        return Op{ OpClass::RV32A, OpCode::amoadd_w, DecodeOperandR(insn) };
    case 0b00100:
        return Op{ OpClass::RV32A, OpCode::amoxor_w, DecodeOperandR(insn) };
    case 0b01100:
        return Op{ OpClass::RV32A, OpCode::amoand_w, DecodeOperandR(insn) };
    case 0b01000:
        return Op{ OpClass::RV32A, OpCode::amoor_w, DecodeOperandR(insn) };
    case 0b10000:
        return Op{ OpClass::RV32A, OpCode::amomin_w, DecodeOperandR(insn) };
    case 0b10100:
        return Op{ OpClass::RV32A, OpCode::amomax_w, DecodeOperandR(insn) };
    case 0b11000:
        return Op{ OpClass::RV32A, OpCode::amominu_w, DecodeOperandR(insn) };
    case 0b11100:
        return Op{ OpClass::RV32A, OpCode::amomaxu_w, DecodeOperandR(insn) };
    default:
        return Op{ OpClass::RV32A, OpCode::unknown, OperandNone() };
    }
}

Op DecoderImpl::DecodeRV32F(uint32_t insn) const
{
    const auto opcode = Pick(insn, 0, 7);
    const auto funct3 = Pick(insn, 12, 3);
    const auto funct7 = Pick(insn, 25, 7);
    const auto funct2 = Pick(insn, 25, 2);
    const auto rs2 = Pick(insn, 20, 5);

    switch (opcode)
    {
    case 0b0000111:
        switch (funct3)
        {
        case 0b010:
            return Op{ OpClass::RV32F, OpCode::flw, DecodeOperandI(insn) };
        default:
            return Op{ OpClass::RV32F, OpCode::unknown, OperandNone() };
        }
    case 0b0100111:
        switch (funct3)
        {
        case 0b010:
            return Op{ OpClass::RV32F, OpCode::fsw, DecodeOperandS(insn) };
        default:
            return Op{ OpClass::RV32F, OpCode::unknown, OperandNone() };
        }
    case 0b1000011:
        return Op{ OpClass::RV32F, OpCode::fmadd_s, DecodeOperandR4(insn) };
    case 0b1000111:
        return Op{ OpClass::RV32F, OpCode::fmsub_s, DecodeOperandR4(insn) };
    case 0b1001011:
        return Op{ OpClass::RV32F, OpCode::fnmsub_s, DecodeOperandR4(insn) };
    case 0b1001111:
        return Op{ OpClass::RV32F, OpCode::fnmadd_s, DecodeOperandR4(insn) };
    case 0b1010011:
        switch (funct7)
        {
        case 0b0000000:
            return Op{ OpClass::RV32F, OpCode::fadd_s, DecodeOperandR(insn) };
        case 0b0000100:
            return Op{ OpClass::RV32F, OpCode::fsub_s, DecodeOperandR(insn) };
        case 0b0001000:
            return Op{ OpClass::RV32F, OpCode::fmul_s, DecodeOperandR(insn) };
        case 0b0001100:
            return Op{ OpClass::RV32F, OpCode::fdiv_s, DecodeOperandR(insn) };
        case 0b0101100:
            switch (rs2)
            {
            case 0b00000:
                return Op{ OpClass::RV32F, OpCode::fsqrt_s, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32F, OpCode::unknown, OperandNone() };
            }
        case 0b0010000:
            switch (funct3)
            {
            case 0b000:
                return Op{ OpClass::RV32F, OpCode::fsgnj_s, DecodeOperandR(insn) };
            case 0b001:
                return Op{ OpClass::RV32F, OpCode::fsgnjn_s, DecodeOperandR(insn) };
            case 0b010:
                return Op{ OpClass::RV32F, OpCode::fsgnjx_s, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32F, OpCode::unknown, OperandNone() };
            }
        case 0b0010100:
            switch (funct3)
            {
            case 0b000:
                return Op{ OpClass::RV32F, OpCode::fmin_s, DecodeOperandR(insn) };
            case 0b001:
                return Op{ OpClass::RV32F, OpCode::fmax_s, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32F, OpCode::unknown, OperandNone() };
            }
        case 0b1100000:
            switch (rs2)
            {
            case 0b00000:
                return Op{ OpClass::RV32F, OpCode::fcvt_w_s, DecodeOperandR(insn) };
            case 0b00001:
                return Op{ OpClass::RV32F, OpCode::fcvt_wu_s, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32F, OpCode::unknown, OperandNone() };
            }
        case 0b1110000:
            if (rs2 == 0b00000 && funct3 == 0b000)
            {
                return Op{ OpClass::RV32F, OpCode::fmv_x_w, DecodeOperandR(insn) };
            }
            else if (rs2 == 0b00000 && funct3 == 0b001)
            {
                return Op{ OpClass::RV32F, OpCode::fclass_s, DecodeOperandR(insn) };
            }
            else
            {
                return Op{ OpClass::RV32F, OpCode::unknown, OperandNone() };
            }
        case 0b1010000:
            switch (funct3)
            {
            case 0b000:
                return Op{ OpClass::RV32F, OpCode::fle_s, DecodeOperandR(insn) };
            case 0b001:
                return Op{ OpClass::RV32F, OpCode::flt_s, DecodeOperandR(insn) };
            case 0b010:
                return Op{ OpClass::RV32F, OpCode::feq_s, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32F, OpCode::unknown, OperandNone() };
            }
        case 0b1101000:
            switch (rs2)
            {
            case 0b00000:
                return Op{ OpClass::RV32F, OpCode::fcvt_s_w, DecodeOperandR(insn) };
            case 0b00001:
                return Op{ OpClass::RV32F, OpCode::fcvt_s_wu, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32F, OpCode::unknown, OperandNone() };
            }
        case 0b1111000:
            if (rs2 == 0b00000 && funct3 == 0b000)
            {
                return Op{ OpClass::RV32F, OpCode::fmv_w_x, DecodeOperandR(insn) };
            }
            else
            {
                return Op{ OpClass::RV32F, OpCode::unknown, OperandNone() };
            }
        default:
            return Op{ OpClass::RV32F, OpCode::unknown, OperandNone() };
        }
    default:
        return Op{ OpClass::RV32F, OpCode::unknown, OperandNone() };
    }
}

Op DecoderImpl::DecodeRV32D(uint32_t insn) const
{
    const auto opcode = Pick(insn, 0, 7);
    const auto funct3 = Pick(insn, 12, 3);
    const auto funct7 = Pick(insn, 25, 7);
    const auto funct2 = Pick(insn, 25, 2);
    const auto rs2 = Pick(insn, 20, 5);

    switch (opcode)
    {
    case 0b0000111:
        switch (funct3)
        {
        case 0b011:
            return Op{ OpClass::RV32D, OpCode::fld, DecodeOperandI(insn) };
        default:
            return Op{ OpClass::RV32D, OpCode::unknown, OperandNone() };
        }
    case 0b0100111:
        switch (funct3)
        {
        case 0b011:
            return Op{ OpClass::RV32D, OpCode::fsd, DecodeOperandS(insn) };
        default:
            return Op{ OpClass::RV32D, OpCode::unknown, OperandNone() };
        }
    case 0b1000011:
        return Op{ OpClass::RV32D, OpCode::fmadd_d, DecodeOperandR4(insn) };
    case 0b1000111:
        return Op{ OpClass::RV32D, OpCode::fmsub_d, DecodeOperandR4(insn) };
    case 0b1001011:
        return Op{ OpClass::RV32D, OpCode::fnmsub_d, DecodeOperandR4(insn) };
    case 0b1001111:
        return Op{ OpClass::RV32D, OpCode::fnmadd_d, DecodeOperandR4(insn) };
    case 0b1010011:
        switch (funct7)
        {
        case 0b0000001:
            return Op{ OpClass::RV32D, OpCode::fadd_d, DecodeOperandR(insn) };
        case 0b0000101:
            return Op{ OpClass::RV32D, OpCode::fsub_d, DecodeOperandR(insn) };
        case 0b0001001:
            return Op{ OpClass::RV32D, OpCode::fmul_d, DecodeOperandR(insn) };
        case 0b0001101:
            return Op{ OpClass::RV32D, OpCode::fdiv_d, DecodeOperandR(insn) };
        case 0b0101101:
            switch (rs2)
            {
            case 0b00000:
                return Op{ OpClass::RV32D, OpCode::fsqrt_d, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32D, OpCode::unknown, OperandNone() };
            }
        case 0b0010001:
            switch (funct3)
            {
            case 0b000:
                return Op{ OpClass::RV32D, OpCode::fsgnj_d, DecodeOperandR(insn) };
            case 0b001:
                return Op{ OpClass::RV32D, OpCode::fsgnjn_d, DecodeOperandR(insn) };
            case 0b010:
                return Op{ OpClass::RV32D, OpCode::fsgnjx_d, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32D, OpCode::unknown, OperandNone() };
            }
        case 0b0010101:
            switch (funct3)
            {
            case 0b000:
                return Op{ OpClass::RV32D, OpCode::fmin_d, DecodeOperandR(insn) };
            case 0b001:
                return Op{ OpClass::RV32D, OpCode::fmax_d, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32D, OpCode::unknown, OperandNone() };
            }
        case 0b0100000:
            switch (rs2)
            {
            case 0b00001:
                return Op{ OpClass::RV32D, OpCode::fcvt_s_d, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32D, OpCode::unknown, OperandNone() };
            }
        case 0b0100001:
            switch (rs2)
            {
            case 0b00000:
                return Op{ OpClass::RV32D, OpCode::fcvt_d_s, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32D, OpCode::unknown, OperandNone() };
            }
        case 0b1010001:
            switch (funct3)
            {
            case 0b000:
                return Op{ OpClass::RV32D, OpCode::fle_d, DecodeOperandR(insn) };
            case 0b001:
                return Op{ OpClass::RV32D, OpCode::flt_d, DecodeOperandR(insn) };
            case 0b010:
                return Op{ OpClass::RV32D, OpCode::feq_d, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32D, OpCode::unknown, OperandNone() };
            }
        case 0b1110001:
            if (rs2 == 0b00000 && funct3 == 0b001)
            {
                return Op{ OpClass::RV32D, OpCode::fclass_d, DecodeOperandR(insn) };
            }
            else
            {
                return Op{ OpClass::RV32D, OpCode::unknown, OperandNone() };
            }
        case 0b1100001:
            switch (rs2)
            {
            case 0b00000:
                return Op{ OpClass::RV32D, OpCode::fcvt_w_d, DecodeOperandR(insn) };
            case 0b00001:
                return Op{ OpClass::RV32D, OpCode::fcvt_wu_d, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32D, OpCode::unknown, OperandNone() };
            }
        case 0b1101001:
            switch (rs2)
            {
            case 0b00000:
                return Op{ OpClass::RV32D, OpCode::fcvt_d_w, DecodeOperandR(insn) };
            case 0b00001:
                return Op{ OpClass::RV32D, OpCode::fcvt_d_wu, DecodeOperandR(insn) };
            default:
                return Op{ OpClass::RV32D, OpCode::unknown, OperandNone() };
            }
        default:
            return Op{ OpClass::RV32D, OpCode::unknown, OperandNone() };
        }
    default:
        return Op{ OpClass::RV32D, OpCode::unknown, OperandNone() };
    }
}

Op DecoderImpl::DecodeRV32C(uint16_t insn) const
{
    const auto opcode = Pick(insn, 0, 2);
    const auto funct4 = Pick(insn, 12, 4);
    const auto funct3 = Pick(insn, 13, 3);
    const auto funct2_rs1 = Pick(insn, 10, 2);
    const auto funct2_rs2 = Pick(insn, 5, 2);
    const auto funct1 = Pick(insn, 12);
    
    const auto rd = Pick(insn, 7, 5);
    const auto rs1 = Pick(insn, 7, 5);
    const auto rs2 = Pick(insn, 2, 5);

    switch (opcode)
    {
    case 0b00:
        if (funct3 == 0b000 && rs1 != 0)
        {
            return Op{ OpClass::RV32C, OpCode::c_addi4spn, DecodeOperandCIW(insn) };
        }
        else if (funct3 == 0b001)
        {
            return Op{ OpClass::RV32C, OpCode::c_fld, DecodeOperandCL(insn, 8) };
        }
        else if (funct3 == 0b010)
        {
            return Op{ OpClass::RV32C, OpCode::c_lw, DecodeOperandCL(insn, 4) };
        }
        else if (funct3 == 0b011)
        {
            return Op{ OpClass::RV32C, OpCode::c_flw, DecodeOperandCL(insn, 4) };
        }
        else if (funct3 == 0b101)
        {
            return Op{ OpClass::RV32C, OpCode::c_fsd, DecodeOperandCS(insn, 8) };
        }
        else if (funct3 == 0b110)
        {
            return Op{ OpClass::RV32C, OpCode::c_sw, DecodeOperandCS(insn, 4) };
        }
        else if (funct3 == 0b111)
        {
            return Op{ OpClass::RV32C, OpCode::c_fsw, DecodeOperandCS(insn, 4) };
        }
        else
        {
            return Op{ OpClass::RV32C, OpCode::unknown, OperandNone() };
        }
    case 0b01:
        if (funct4 == 0b0000 && rd == 0 && rs2 == 0)
        {
            return Op{ OpClass::RV32C, OpCode::c_nop, OperandNone() };
        }
        else if (funct3 == 0b000)
        {
            return Op{ OpClass::RV32C, OpCode::c_addi, DecodeOperandCI(insn, true) };
        }
        else if (funct3 == 0b001)
        {
            return Op{ OpClass::RV32C, OpCode::c_jal, DecodeOperandCJ(insn) };
        }
        else if (funct3 == 0b010 && rd != 0)
        {
            return Op{ OpClass::RV32C, OpCode::c_li, DecodeOperandCI(insn, true) };
        }
        else if (funct3 == 0b011 && rd == 2)
        {
            return Op{ OpClass::RV32C, OpCode::c_addi16sp, DecodeOperandCI_ADDI16SP(insn) };
        }
        else if (funct3 == 0b011 && rd != 0 && rd != 2)
        {
            return Op{ OpClass::RV32C, OpCode::c_lui, DecodeOperandCI_LUI(insn) };
        }
        else if (funct3 == 0b100 && funct2_rs1 == 0b00)
        {
            return Op{ OpClass::RV32C, OpCode::c_srli, DecodeOperandCI_AluImm(insn, false) };
        }
        else if (funct3 == 0b100 && funct2_rs1 == 0b01)
        {
            return Op{ OpClass::RV32C, OpCode::c_srai, DecodeOperandCI_AluImm(insn, false) };
        }
        else if (funct3 == 0b100 && funct2_rs1 == 0b10)
        {
            return Op{ OpClass::RV32C, OpCode::c_andi, DecodeOperandCI_AluImm(insn, true) };
        }
        else if (funct4 == 0b1000 && funct2_rs1 == 0b11 && funct2_rs2 == 0b00)
        {
            return Op{ OpClass::RV32C, OpCode::c_sub, DecodeOperandCR_Alu(insn) };
        }
        else if (funct4 == 0b1000 && funct2_rs1 == 0b11 && funct2_rs2 == 0b01)
        {
            return Op{ OpClass::RV32C, OpCode::c_xor, DecodeOperandCR_Alu(insn) };
        }
        else if (funct4 == 0b1000 && funct2_rs1 == 0b11 && funct2_rs2 == 0b10)
        {
            return Op{ OpClass::RV32C, OpCode::c_or, DecodeOperandCR_Alu(insn) };
        }
        else if (funct4 == 0b1000 && funct2_rs1 == 0b11 && funct2_rs2 == 0b11)
        {
            return Op{ OpClass::RV32C, OpCode::c_and, DecodeOperandCR_Alu(insn) };
        }
        else if (funct3 == 0b101)
        {
            return Op{ OpClass::RV32C, OpCode::c_j, DecodeOperandCJ(insn) };
        }
        else if (funct3 == 0b110)
        {
            return Op{ OpClass::RV32C, OpCode::c_beqz, DecodeOperandCB(insn) };
        }
        else if (funct3 == 0b111)
        {
            return Op{ OpClass::RV32C, OpCode::c_bnez, DecodeOperandCB(insn) };
        }
        else
        {
            return Op{ OpClass::RV32C, OpCode::unknown, OperandNone() };
        }
    case 0b10:
        if (funct4 == 0b000)
        {
            return Op{ OpClass::RV32C, OpCode::c_slli, DecodeOperandCI(insn, true) };
        }
        else if (funct3 == 0b001)
        {
            return Op{ OpClass::RV32C, OpCode::c_fldsp, DecodeOperandCI_LoadSP(insn, 8) };
        }
        else if (funct3 == 0b010 && rd != 0)
        {
            return Op{ OpClass::RV32C, OpCode::c_lwsp, DecodeOperandCI_LoadSP(insn, 4) };
        }
        else if (funct3 == 0b011)
        {
            return Op{ OpClass::RV32C, OpCode::c_flwsp, DecodeOperandCI_LoadSP(insn, 4) };
        }
        else if (funct4 == 0b1000 && rs1 != 0 && rs2 == 0)
        {
            return Op{ OpClass::RV32C, OpCode::c_jr, DecodeOperandCR(insn) };
        }
        else if (funct4 == 0b1000 && rd != 0 && rs2 != 0)
        {
            return Op{ OpClass::RV32C, OpCode::c_mv, DecodeOperandCR(insn) };
        }
        else if (funct4 == 0b1001 && rd == 0 && rs2 == 0)
        {
            return Op{ OpClass::RV32C, OpCode::c_ebreak, DecodeOperandCR(insn) };
        }
        else if (funct4 == 0b1001 && rs1 != 0 && rs2 == 0)
        {
            return Op{ OpClass::RV32C, OpCode::c_jalr, DecodeOperandCR(insn) };
        }
        else if (funct4 == 0b1001 && rs1 != 0 && rs2 != 0)
        {
            return Op{ OpClass::RV32C, OpCode::c_add, DecodeOperandCR(insn) };
        }
        else if (funct3 == 0b101)
        {
            return Op{ OpClass::RV32C, OpCode::c_fsdsp, DecodeOperandCSS(insn, 8) };
        }
        else if (funct3 == 0b110)
        {
            return Op{ OpClass::RV32C, OpCode::c_swsp, DecodeOperandCSS(insn, 4) };
        }
        else if (funct3 == 0b111)
        {
            return Op{ OpClass::RV32C, OpCode::c_fswsp, DecodeOperandCSS(insn, 4) };
        }
        else
        {
            return Op{ OpClass::RV32C, OpCode::unknown, OperandNone() };
        }
    default:
        return Op{ OpClass::RV32C, OpCode::unknown, OperandNone() };
    }
}

Operand DecoderImpl::DecodeOperandR(uint32_t insn) const
{
    return Operand(OperandR
    {
        Pick(insn, 7, 5), // rd
        Pick(insn, 15, 5), // rs1
        Pick(insn, 20, 5), // rs2
        Pick(insn, 12, 3), // funct3
        Pick(insn, 25, 5), // funct7
    });
}

Operand DecoderImpl::DecodeOperandR4(uint32_t insn) const
{
    return Operand(OperandR4
    {
        Pick(insn, 7, 5), // rd
        Pick(insn, 15, 5), // rs1
        Pick(insn, 20, 5), // rs2
        Pick(insn, 27, 5), // rs3
        Pick(insn, 12, 3), // funct3
        Pick(insn, 25, 2), // funct2
    });
}

Operand DecoderImpl::DecodeOperandI(uint32_t insn) const
{
    return Operand(OperandI
    {
        sext(Pick(insn, 20, 12), 11), // imm
        Pick(insn, 7, 5), // rd
        Pick(insn, 15, 5), // rs1
        Pick(insn, 12, 3), // funct3
    });
}

Operand DecoderImpl::DecodeOperandS(uint32_t insn) const
{
    return Operand(OperandS
    {
        sext(Pick(insn, 25, 7) << 5 | Pick(insn, 7, 5), 11), // imm
        Pick(insn, 15, 5), // rs1
        Pick(insn, 20, 5), // rs2
        Pick(insn, 12, 3), // funct3
    });
}

Operand DecoderImpl::DecodeOperandB(uint32_t insn) const
{
    return Operand(OperandB
    {
        sext(Pick(insn, 31, 1) << 12 | Pick(insn, 25, 6) << 5 | Pick(insn, 8, 4) << 1 | Pick(insn, 7, 1) << 11, 12), // imm
        Pick(insn, 15, 5), // rs1
        Pick(insn, 20, 5), // rs2
        Pick(insn, 12, 3), // funct3
    });
}

Operand DecoderImpl::DecodeOperandU(uint32_t insn) const
{
    return Operand(OperandU
    {
        Pick(insn, 12, 20) << 12, // imm
        Pick(insn, 7, 5), // rd
    });
}

Operand DecoderImpl::DecodeOperandJ(uint32_t insn) const
{
    return Operand(OperandJ
    {
        sext(Pick(insn, 31, 1) << 20 | Pick(insn, 21, 10) << 1 | Pick(insn, 20, 1) << 11 | Pick(insn, 12, 8) << 12, 20), // imm
        Pick(insn, 7, 5), // rd
    });
}

Operand DecoderImpl::DecodeOperandShiftImm(uint32_t insn) const
{
    return Operand(OperandShiftImm
    {
        Pick(insn, 7, 5), // rd
        Pick(insn, 15, 5), // rs1
        Pick(insn, 20, 5), // shamt
    });
}

Operand DecoderImpl::DecodeOperandCsr(uint32_t insn) const
{
    return Operand(OperandCsr
    {
        Pick(insn, 7, 5), // rd
        Pick(insn, 15, 5), // rs1
        static_cast<csr_addr_t>(Pick(insn, 20, 12)), // csr
    });
}

Operand DecoderImpl::DecodeOperandCsrImm(uint32_t insn) const
{
    return Operand(OperandCsrImm
    {
        Pick(insn, 15, 5), // zimm
        Pick(insn, 7, 5), // rd
        static_cast<csr_addr_t>(Pick(insn, 20, 12)), // csr
    });
}

Operand DecoderImpl::DecodeOperandFence(uint32_t insn) const
{
    return Operand(OperandFence
    {
        Pick(insn, 24, 4), // pred
        Pick(insn, 20, 4), // succ
    });
}

Operand DecoderImpl::DecodeOperandCR(uint16_t insn) const
{
    return Operand(OperandCR
    {
        Pick(insn, 7, 5), // rd
        Pick(insn, 7, 5), // rs1
        Pick(insn, 2, 5), // rs2
    });
}

Operand DecoderImpl::DecodeOperandCR_Alu(uint16_t insn) const
{
    return Operand(OperandCR
    {
        Pick(insn, 7, 3) + 8, // rd
        Pick(insn, 7, 3) + 8, // rs1
        Pick(insn, 2, 3) + 8, // rs2
    });
}

Operand DecoderImpl::DecodeOperandCI(uint16_t insn, bool immSigned) const
{
    return Operand(OperandCI
    {
        immSigned ?
            SignExtend(6,
                Pick(insn, 12, 1) << 5 |
                Pick(insn, 2, 5)
            ) :
            ZeroExtend(6,
                Pick(insn, 12, 1) << 5 |
                Pick(insn, 2, 5)
            ), // imm
        Pick(insn, 7, 5), // rd
        Pick(insn, 7, 5), // rs1
    });
}

Operand DecoderImpl::DecodeOperandCI_ADDI16SP(uint16_t insn) const
{
    return Operand(OperandCI
    {
        SignExtend(10,
            Pick(insn, 12) << 9 |
            Pick(insn, 6) << 4 |
            Pick(insn, 5) << 6 |
            Pick(insn, 3, 2) << 7 |
            Pick(insn, 2) << 5
        ), // imm
        Pick(insn, 7, 5), // rd
        Pick(insn, 7, 5), // rs1
    });
}

Operand DecoderImpl::DecodeOperandCI_AluImm(uint16_t insn, bool immSigned) const
{
    return Operand(OperandCI
    {
        immSigned ?
            SignExtend(6,
                Pick(insn, 12, 1) << 5 |
                Pick(insn, 2, 5)
            ) :
            ZeroExtend(6,
                Pick(insn, 12, 1) << 5 |
                Pick(insn, 2, 5)
            ), // imm
        Pick(insn, 7, 3) + 8, // rd
        Pick(insn, 7, 3) + 8, // rs1
    });
}

Operand DecoderImpl::DecodeOperandCI_LoadSP(uint16_t insn, int accessSize) const
{
    switch (accessSize)
    {
    case 4:
        return Operand(OperandCI
        {
            ZeroExtend(8,
                Pick(insn, 12) << 5 |
                Pick(insn, 4, 3) << 2 |
                Pick(insn, 2, 2) << 6
            ), // imm
            Pick(insn, 7, 5), // rd
            Pick(insn, 7, 5), // rs1
        });
    case 8:
        return Operand(OperandCI
        {
            ZeroExtend(9,
                Pick(insn, 12) << 5 |
                Pick(insn, 5, 2) << 3 |
                Pick(insn, 2, 3) << 6
            ), // imm
            Pick(insn, 7, 5), // rd
            Pick(insn, 7, 5), // rs1
        });
    case 16:
        return Operand(OperandCI
        {
            ZeroExtend(10,
                Pick(insn, 12) << 5 |
                Pick(insn, 6, 1) << 4 |
                Pick(insn, 2, 4) << 6
            ), // imm
            Pick(insn, 7, 5), // rd
            Pick(insn, 7, 5), // rs1
        });
    default:
        RAFI_NOT_IMPLEMENTED();
    }
}

Operand DecoderImpl::DecodeOperandCI_LUI(uint16_t insn) const
{
    return Operand(OperandCI
    {
        SignExtend(18,
            Pick(insn, 12) << 17 |
            Pick(insn, 2, 5) << 12
        ), // imm
        Pick(insn, 7, 5), // rd
        Pick(insn, 7, 5), // rs1
    });
}

Operand DecoderImpl::DecodeOperandCSS(uint16_t insn, int accessSize) const
{
    switch (accessSize)
    {
    case 4:
        return Operand(OperandCSS
        {
            ZeroExtend(6,
                Pick(insn, 9, 4) << 2 |
                Pick(insn, 7, 2) << 6
            ), // imm
            Pick(insn, 2, 5), // rs2
        });
    case 8:
        return Operand(OperandCSS
        {
            ZeroExtend(6,
                Pick(insn, 10, 3) << 3 |
                Pick(insn, 7, 3) << 6
            ), // imm
            Pick(insn, 2, 5), // rs2
        });
    case 16:
        return Operand(OperandCSS
        {
            ZeroExtend(6,
                Pick(insn, 11, 2) << 4 |
                Pick(insn, 7, 4) << 6
            ), // imm
            Pick(insn, 2, 5), // rs2
        });
    default:
        RAFI_NOT_IMPLEMENTED();
    }
}

Operand DecoderImpl::DecodeOperandCIW(uint16_t insn) const
{
    return Operand(OperandCIW
    {
        ZeroExtend(10,
            Pick(insn, 11, 2) << 4 |
            Pick(insn, 7, 4) << 6 |
            Pick(insn, 6) << 2 |
            Pick(insn, 5) << 3
        ), // imm
        Pick(insn, 2, 3) + 8, // rd
    });
}

Operand DecoderImpl::DecodeOperandCL(uint16_t insn, int accessSize) const
{
    switch (accessSize)
    {
    case 4:
        return Operand(OperandCL
        {
            ZeroExtend(7,
                Pick(insn, 10, 3) << 3 |
                Pick(insn, 6) << 2 |
                Pick(insn, 5) << 6
            ), // imm
            Pick(insn, 2, 3) + 8, // rd
            Pick(insn, 7, 3) + 8, // rs1
        });
    case 8:
        return Operand(OperandCL
        {
            ZeroExtend(7,
                Pick(insn, 10, 3) << 3 |
                Pick(insn, 5, 2) << 6
            ), // imm
            Pick(insn, 2, 3) + 8, // rd
            Pick(insn, 7, 3) + 8, // rs1
        });
    case 16:
        return Operand(OperandCL
        {
            ZeroExtend(7,
                Pick(insn, 11, 2) << 4 |
                Pick(insn, 10) << 8 |
                Pick(insn, 5, 2) << 6
            ), // imm
            Pick(insn, 2, 3) + 8, // rd
            Pick(insn, 7, 3) + 8, // rs1
        });
    default:
        RAFI_NOT_IMPLEMENTED();
    }
}

Operand DecoderImpl::DecodeOperandCS(uint16_t insn, int accessSize) const
{
    switch (accessSize)
    {
    case 4:
        return Operand(OperandCS
        {
            ZeroExtend(7,
                Pick(insn, 10, 3) << 3 |
                Pick(insn, 6) << 2 |
                Pick(insn, 5) << 6
            ), // imm
            Pick(insn, 7, 3) + 8, // rs1
            Pick(insn, 2, 3) + 8, // rs2
        });
    case 8:
        return Operand(OperandCS
        {
            ZeroExtend(7,
                Pick(insn, 10, 3) << 3 |
                Pick(insn, 5, 2) << 6
            ), // imm
            Pick(insn, 7, 3) + 8, // rs1
            Pick(insn, 2, 3) + 8, // rs2
        });
    case 16:
        return Operand(OperandCS
        {
            ZeroExtend(7,
                Pick(insn, 11, 2) << 4 |
                Pick(insn, 10) << 8 |
                Pick(insn, 5, 2) << 6
            ), // imm
            Pick(insn, 7, 3) + 8, // rs1
            Pick(insn, 2, 3) + 8, // rs2
        });
    default:
        RAFI_NOT_IMPLEMENTED();
    }
}

Operand DecoderImpl::DecodeOperandCB(uint16_t insn) const
{
    return Operand(OperandCB
    {
        SignExtend(12,
            Pick(insn, 12, 1) << 8 |
            Pick(insn, 10, 2) << 3 |
            Pick(insn, 5, 2) << 6 |
            Pick(insn, 3, 2) << 1 |
            Pick(insn, 2, 1) << 5
        ), // imm
        Pick(insn, 7, 3) + 8, // rs1
    });
}

Operand DecoderImpl::DecodeOperandCJ(uint16_t insn) const
{
    return Operand(OperandCJ
    {
        SignExtend(12,
            Pick(insn, 12, 1) << 11 |
            Pick(insn, 11, 1) << 4 |
            Pick(insn, 9, 2) << 8 |
            Pick(insn, 8, 1) << 10 |
            Pick(insn, 7, 1) << 6 |
            Pick(insn, 6, 1) << 7 |
            Pick(insn, 3, 3) << 1 |
            Pick(insn, 2, 1) << 5
        ), // imm
    });
}

}
