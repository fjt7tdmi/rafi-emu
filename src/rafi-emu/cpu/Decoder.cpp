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
#include <cstdint>
#include <fstream>
#include <iostream>

#include <rvtrace/common.h>

#include "Decoder.h"
#include "Trap.h"

using namespace std;
using namespace rvtrace;

namespace rafi { namespace cpu {

namespace
{
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

    inline int32_t Pick(int32_t insn, int lsb, int width)
    {
        assert(0 <= lsb && lsb < 32);
        assert(1 <= width && width < 32);
        return (insn >> lsb) & ((1 << width) - 1);
    }
}

void Decoder::Decode(Op* out, int32_t insn) const
{
    assert(out != nullptr);

    const auto opcode = Pick(insn, 0, 7);
    const auto funct3 = Pick(insn, 12, 3);
    const auto funct7 = Pick(insn, 25, 7);

    if (opcode == 0b0110011 && funct7 == 0b0000001)
    {
        DecodeRV32M(out, insn);
    }
    else if (opcode == 0b0101111 && funct3 == 0b010)
    {
        DecodeRV32A(out, insn);
    }
    else
    {
        DecodeRV32I(out, insn);
    }
}

void Decoder::DecodeRV32I(Op* out, int32_t insn) const
{
    const auto opcode = Pick(insn,  0, 7);
    const auto funct3 = Pick(insn, 12, 3);
    const auto funct7 = Pick(insn, 25, 7);
    const auto funct12 = Pick(insn, 20, 12);

    const auto rd   = Pick(insn,  7,  5);
    const auto rs1  = Pick(insn, 15,  5);
    const auto rs2  = Pick(insn, 20,  5);
    const auto csr  = Pick(insn, 20, 12);
    const auto pred = Pick(insn, 24,  4);
    const auto succ = Pick(insn, 20,  4);

    const auto zimm = rs1;
    const auto shamt = rs2;

    // First, set default value
    out->opCode = OpCode::unknown;
    out->opClass = OpClass::RV32I;
    out->rd = rd;
    out->rs1 = rs1;
    out->rs2 = rs2;
    out->csr = csr;
    out->pred = pred;
    out->succ = succ;
    out->zimm = zimm;
    out->shamt = shamt;
    out->aq = 0;
    out->rl = 0;

    switch (opcode)
    {
    case 0b0110111:
        out->opCode = OpCode::lui;
        out->imm = Pick(insn, 12, 20) << 12;
        break;
    case 0b0010111:
        out->opCode = OpCode::auipc;
        out->imm = Pick(insn, 12, 20) << 12;
        break;
    case 0b1101111:
        out->opCode = OpCode::jal;
        out->imm = sext(Pick(insn, 31, 1) << 20 | Pick(insn, 21, 10) << 1 | Pick(insn, 20, 1) << 11 | Pick(insn, 12, 8) << 12, 20);
        break;
    case 0b1100111:
        if (funct3 == 0)
        {
            out->opCode = OpCode::jalr;
            out->imm = sext(Pick(insn, 20, 12), 11);
        }
        break;
    case 0b1100011:
        if (funct3 == 0)
        {
            out->opCode = OpCode::beq;
        }
        else if (funct3 == 1)
        {
            out->opCode = OpCode::bne;
        }
        else if (funct3 == 4)
        {
            out->opCode = OpCode::blt;
        }
        else if (funct3 == 5)
        {
            out->opCode = OpCode::bge;
        }
        else if (funct3 == 6)
        {
            out->opCode = OpCode::bltu;
        }
        else if (funct3 == 7)
        {
            out->opCode = OpCode::bgeu;
        }
        out->imm = sext(Pick(insn, 31, 1) << 12 | Pick(insn, 25, 6) << 5 | Pick(insn, 8, 4) << 1 | Pick(insn, 7, 1) << 11, 12);
        break;
    case 0b0000011:
        if (funct3 == 0)
        {
            out->opCode = OpCode::lb;
        }
        else if (funct3 == 1)
        {
            out->opCode = OpCode::lh;
        }
        else if (funct3 == 2)
        {
            out->opCode = OpCode::lw;
        }
        else if (funct3 == 4)
        {
            out->opCode = OpCode::lbu;
        }
        else if (funct3 == 5)
        {
            out->opCode = OpCode::lhu;
        }
        out->imm = sext(Pick(insn, 20, 12), 11);
        break;
    case 0b0100011:
        if (funct3 == 0)
        {
            out->opCode = OpCode::sb;
        }
        else if (funct3 == 1)
        {
            out->opCode = OpCode::sh;
        }
        else if (funct3 == 2)
        {
            out->opCode = OpCode::sw;
        }
        out->imm = sext(Pick(insn, 25, 7) << 5 | Pick(insn, 7, 5), 11);
        break;
    case 0b0010011:
        if (funct3 == 0)
        {
            out->opCode = OpCode::addi;
        }
        else if (funct3 == 1 && funct7 == 0b0000000)
        {
            out->opCode = OpCode::slli;
        }
        else if (funct3 == 2)
        {
            out->opCode = OpCode::slti;
        }
        else if (funct3 == 3)
        {
            out->opCode = OpCode::sltiu;
        }
        else if (funct3 == 4)
        {
            out->opCode = OpCode::xori;
        }
        else if (funct3 == 5 && funct7 == 0b0000000)
        {
            out->opCode = OpCode::srli;
        }
        else if (funct3 == 5 && funct7 == 0b0100000)
        {
            out->opCode = OpCode::srai;
        }
        else if (funct3 == 6)
        {
            out->opCode = OpCode::ori;
        }
        else if (funct3 == 7)
        {
            out->opCode = OpCode::andi;
        }
        out->imm = sext(Pick(insn, 20, 12), 11);
        break;
    case 0b0110011:
        if (funct7 == 0b0000000)
        {
            // RV32I
            if (funct3 == 0)
            {
                out->opCode = OpCode::add;
            }
            else if (funct3 == 1)
            {
                out->opCode = OpCode::sll;
            }
            else if (funct3 == 2)
            {
                out->opCode = OpCode::slt;
            }
            else if (funct3 == 3)
            {
                out->opCode = OpCode::sltu;
            }
            else if (funct3 == 4)
            {
                out->opCode = OpCode::xor_;
            }
            else if (funct3 == 5)
            {
                out->opCode = OpCode::srl;
            }
            else if (funct3 == 6)
            {
                out->opCode = OpCode::or_;
            }
            else if (funct3 == 7)
            {
                out->opCode = OpCode::and_;
            }
        }
        else if (funct7 == 0b0000001)
        {
            // RV32M
            if (funct3 == 0)
            {
                out->opCode = OpCode::mul;
            }
            else if (funct3 == 1)
            {
                out->opCode = OpCode::mulh;
            }
            else if (funct3 == 2)
            {
                out->opCode = OpCode::mulhsu;
            }
            else if (funct3 == 3)
            {
                out->opCode = OpCode::mulhu;
            }
            else if (funct3 == 4)
            {
                out->opCode = OpCode::div;
            }
            else if (funct3 == 5)
            {
                out->opCode = OpCode::divu;
            }
            else if (funct3 == 6)
            {
                out->opCode = OpCode::rem;
            }
            else if (funct3 == 7)
            {
                out->opCode = OpCode::remu;
            }
        }
        else if (funct7 == 0b0100000)
        {
            // RV32I
            if (funct3 == 0)
            {
                out->opCode = OpCode::sub;
            }
            if (funct3 == 5)
            {
                out->opCode = OpCode::sra;
            }
        }
        break;
    case 0b0001111:
        if (funct3 == 0 && rd == 0 && rs1 == 0 && Pick(insn, 28, 4) == 0)
        {
            out->opCode = OpCode::fence_i;
        }
        else if ( funct3 == 1 && rd == 0 && rs1 == 0 && pred == 0 && succ == 0 && Pick(insn, 28, 4) == 0)
        {
            out->opCode = OpCode::fence;
        }
        break;
    case 0b1110011:
        if (funct3 == 0 && rd == 0)
        {
            if (funct7 == 0b0001001)
            {
                out->opCode = OpCode::sfence_vma;
            }
            else if (rs1 == 0)
            {
                switch (funct12)
                {
                case 0b000000000000:
                    out->opCode = OpCode::ecall;
                    break;
                case 0b000000000001:
                    out->opCode = OpCode::ebreak;
                    break;
                case 0b000000000010:
                    out->opCode = OpCode::uret;
                    break;
                case 0b000100000010:
                    out->opCode = OpCode::sret;
                    break;
                case 0b000100000101:
                    out->opCode = OpCode::wfi;
                    break;
                case 0b001100000010:
                    out->opCode = OpCode::mret;
                    break;
                default:
                    break;
                }
            }
        }
        else if (funct3 == 1)
        {
            out->opCode = OpCode::csrrw;
        }
        else if (funct3 == 2)
        {
            out->opCode = OpCode::csrrs;
        }
        else if (funct3 == 3)
        {
            out->opCode = OpCode::csrrc;
        }
        else if (funct3 == 5)
        {
            out->opCode = OpCode::csrrwi;
        }
        else if (funct3 == 6)
        {
            out->opCode = OpCode::csrrsi;
        }
        else if (funct3 == 7)
        {
            out->opCode = OpCode::csrrci;
        }
        break;
    default:
        break;
    }
}

void Decoder::DecodeRV32M(Op* out, int32_t insn) const
{
    const auto funct3 = Pick(insn, 12, 3);

    const auto rd = Pick(insn, 7, 5);
    const auto rs1 = Pick(insn, 15, 5);
    const auto rs2 = Pick(insn, 20, 5);

    // First, set default value
    out->opCode = OpCode::unknown;
    out->opClass = OpClass::RV32M;
    out->rd = rd;
    out->rs1 = rs1;
    out->rs2 = rs2;
    out->aq = 0;
    out->rl = 0;
    out->csr = 0;
    out->pred = 0;
    out->succ = 0;
    out->zimm = 0;
    out->shamt = 0;

    switch (funct3)
    {
    case 0:
        out->opCode = OpCode::mul;
        break;
    case 1:
        out->opCode = OpCode::mulh;
        break;
    case 2:
        out->opCode = OpCode::mulhsu;
        break;
    case 3:
        out->opCode = OpCode::mulhu;
        break;
    case 4:
        out->opCode = OpCode::div;
        break;
    case 5:
        out->opCode = OpCode::divu;
        break;
    case 6:
        out->opCode = OpCode::rem;
        break;
    case 7:
        out->opCode = OpCode::remu;
        break;
    default:
        break;
    }
}

void Decoder::DecodeRV32A(Op* out, int32_t insn) const
{
    const auto funct5 = Pick(insn, 27, 5);
    const auto rd = Pick(insn, 7, 5);
    const auto rs1 = Pick(insn, 15, 5);
    const auto rs2 = Pick(insn, 20, 5);

    out->opClass = OpClass::RV32A;
    out->opCode = OpCode::unknown;
    out->rd = rd;
    out->rs1 = rs1;
    out->rs2 = rs2;
    out->csr = 0;
    out->pred = 0;
    out->succ = 0;
    out->zimm = 0;
    out->shamt = 0;
    out->aq = 0;
    out->rl = 0;

    switch (funct5)
    {
    case 0b00010:
        if (rs2 == 0b00000)
        {
            out->opCode = OpCode::lr_w;
        }
        break;
    case 0b00011:
        out->opCode = OpCode::sc_w;
        break;
    case 0b00001:
        out->opCode = OpCode::amoswap_w;
        break;
    case 0b00000:
        out->opCode = OpCode::amoadd_w;
        break;
    case 0b00100:
        out->opCode = OpCode::amoxor_w;
        break;
    case 0b01100:
        out->opCode = OpCode::amoand_w;
        break;
    case 0b01000:
        out->opCode = OpCode::amoor_w;
        break;
    case 0b10000:
        out->opCode = OpCode::amomin_w;
        break;
    case 0b10100:
        out->opCode = OpCode::amomax_w;
        break;
    case 0b11000:
        out->opCode = OpCode::amominu_w;
        break;
    case 0b11100:
        out->opCode = OpCode::amomaxu_w;
        break;
    default:
        break;
    }
}

void Decoder::DumpOpCode(std::fstream& file, const Op& op) const
{
	file << GetString(op.opCode);
}

void Decoder::DumpOperand(std::fstream& file, const Op& op) const
{
	file << "rd=" << std::dec << op.rd
		<< " rs1=" << std::dec << op.rs1
		<< " rs2=" << std::dec << op.rs2
		<< " imm=0x" << std::hex << op.imm;
}

}}
