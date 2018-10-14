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

#include <cassert>
#include <cstdio>
#include <cstring>
#include <variant>

#include "DecoderImpl.h"

namespace rvtrace {

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

inline int32_t Pick(int32_t insn, int lsb, int width)
{
    assert(0 <= lsb && lsb < 32);
    assert(1 <= width && width < 32);
    return (insn >> lsb) & ((1 << width) - 1);
}

}

const op_t DecoderImpl::Decode(int32_t insn) const
{
    const auto opcode = Pick(insn, 0, 7);
    const auto funct3 = Pick(insn, 12, 3);
    const auto funct7 = Pick(insn, 25, 7);

    if (opcode == 0b0110011 && funct7 == 0b0000001)
    {
        return DecodeRV32M(insn);
    }
    else if (opcode == 0b0101111 && funct3 == 0b010)
    {
        return DecodeRV32A(insn);
    }
    else
    {
        return DecodeRV32I(insn);
    }
}

const op_t DecoderImpl::DecodeRV32I(int32_t insn) const
{
    op_t op;

    const auto opcode = Pick(insn,  0, 7);
    const auto funct3 = Pick(insn, 12, 3);
    const auto funct7 = Pick(insn, 25, 7);
    const auto funct12 = Pick(insn, 20, 12);

    const auto rd   = Pick(insn,  7,  5);
    const auto rs1  = Pick(insn, 15,  5);
    const auto rs2  = Pick(insn, 20,  5);
    const auto csr  = static_cast<csr_addr_t>(Pick(insn, 20, 12));
    const auto pred = Pick(insn, 24,  4);
    const auto succ = Pick(insn, 20,  4);

    const auto zimm = rs1;
    const auto shamt = rs2;

    const auto imm_upper = Pick(insn, 12, 20) << 12;
    const auto imm_jal = sext(Pick(insn, 31, 1) << 20 | Pick(insn, 21, 10) << 1 | Pick(insn, 20, 1) << 11 | Pick(insn, 12, 8) << 12, 20);
    const auto imm_branch = sext(Pick(insn, 31, 1) << 12 | Pick(insn, 25, 6) << 5 | Pick(insn, 8, 4) << 1 | Pick(insn, 7, 1) << 11, 12);
    const auto imm_alu = sext(Pick(insn, 20, 12), 11);
    const auto imm_store = sext(Pick(insn, 25, 7) << 5 | Pick(insn, 7, 5), 11);

    switch (opcode)
    {
    case 0b0110111:
        op = op_lui_t { rd, imm_upper };
        break;
    case 0b0010111:
        op = op_auipc_t { rd, imm_upper };
        break;
    case 0b1101111:
        op = op_jal_t { rd, imm_jal };
        break;
    case 0b1100111:
        if (funct3 == 0)
        {
            op = op_jalr_t { rd, rs1, imm_alu };
        }
        break;
    case 0b1100011:
        if (funct3 == 0)
        {
            op = op_beq_t { rd, rs1, rs2, imm_branch };
        }
        else if (funct3 == 1)
        {
            op = op_bne_t { rd, rs1, rs2, imm_branch };
        }
        else if (funct3 == 4)
        {
            op = op_blt_t { rd, rs1, rs2, imm_branch };
        }
        else if (funct3 == 5)
        {
            op = op_bge_t { rd, rs1, rs2, imm_branch };
        }
        else if (funct3 == 6)
        {
            op = op_bltu_t { rd, rs1, rs2, imm_branch };
        }
        else if (funct3 == 7)
        {
            op = op_bgeu_t { rd, rs1, rs2, imm_branch };
        }
        break;
    case 0b0000011:
        if (funct3 == 0)
        {
            op = op_lb_t { rd, rs1, imm_alu };
        }
        else if (funct3 == 1)
        {
            op = op_lh_t { rd, rs1, imm_alu };
        }
        else if (funct3 == 2)
        {
            op = op_lw_t { rd, rs1, imm_alu };
        }
        else if (funct3 == 4)
        {
            op = op_lbu_t { rd, rs1, imm_alu };
        }
        else if (funct3 == 5)
        {
            op = op_lhu_t { rd, rs1, imm_alu };
        }
        break;
    case 0b0100011:
        if (funct3 == 0)
        {
            op = op_sb_t { rs1, rs2, imm_store };
        }
        else if (funct3 == 1)
        {
            op = op_sh_t { rs1, rs2, imm_store };
        }
        else if (funct3 == 2)
        {
            op = op_sw_t { rs1, rs2, imm_store };
        }
        break;
    case 0b0010011:
        if (funct3 == 0)
        {
            op = op_addi_t { rd, rs1, imm_alu };
        }
        else if (funct3 == 1 && funct7 == 0b0000000)
        {
            op = op_slli_t { rd, rs1, shamt };
        }
        else if (funct3 == 2)
        {
            op = op_slti_t { rd, rs1, imm_alu };
        }
        else if (funct3 == 3)
        {
            op = op_sltiu_t { rd, rs1, imm_alu };
        }
        else if (funct3 == 4)
        {
            op = op_xori_t { rd, rs1, imm_alu };
        }
        else if (funct3 == 5 && funct7 == 0b0000000)
        {
            op = op_srli_t { rd, rs1, shamt };
        }
        else if (funct3 == 5 && funct7 == 0b0100000)
        {
            op = op_srai_t { rd, rs1, shamt };
        }
        else if (funct3 == 6)
        {
            op = op_ori_t { rd, rs1, imm_alu };
        }
        else if (funct3 == 7)
        {
            op = op_andi_t { rd, rs1, imm_alu };
        }
        break;
    case 0b0110011:
        if (funct7 == 0b0000000)
        {
            // RV32I
            if (funct3 == 0)
            {
                op = op_add_t { rd, rs1, rs2 };
            }
            else if (funct3 == 1)
            {
                op = op_sll_t { rd, rs1, rs2 };
            }
            else if (funct3 == 2)
            {
                op = op_slt_t { rd, rs1, rs2 };
            }
            else if (funct3 == 3)
            {
                op = op_sltu_t { rd, rs1, rs2 };
            }
            else if (funct3 == 4)
            {
                op = op_xor_t { rd, rs1, rs2 };
            }
            else if (funct3 == 5)
            {
                op = op_srl_t { rd, rs1, rs2 };
            }
            else if (funct3 == 6)
            {
                op = op_or_t { rd, rs1, rs2 };
            }
            else if (funct3 == 7)
            {
                op = op_and_t { rd, rs1, rs2 };
            }
        }
        else if (funct7 == 0b0100000)
        {
            if (funct3 == 0)
            {
                op = op_sub_t { rd, rs1, rs2 };
            }
            if (funct3 == 5)
            {
                op = op_sra_t { rd, rs1, rs2 };
            }
        }
        break;
    case 0b0001111:
        if (funct3 == 0 && rd == 0 && rs1 == 0 && Pick(insn, 28, 4) == 0)
        {
            op = op_fence_t { pred, succ };
        }
        else if ( funct3 == 1 && rd == 0 && rs1 == 0 && pred == 0 && succ == 0 && Pick(insn, 28, 4) == 0)
        {
            op = op_fencei_t {};
        }
        break;
    case 0b1110011:
        if (funct3 == 0 && rd == 0)
        {
            if (funct7 == 0b0001001)
            {
                op = op_sfencevma_t { rs1, rs2 };
            }
            else if (rs1 == 0)
            {
                switch (funct12)
                {
                case 0b000000000000:
                    op = op_ecall_t {};
                    break;
                case 0b000000000001:
                    op = op_ebreak_t {};
                    break;
                case 0b000000000010:
                    op = op_uret_t {};
                    break;
                case 0b000100000010:
                    op = op_sret_t {};
                    break;
                case 0b000100000101:
                    op = op_wfi_t {};
                    break;
                case 0b001100000010:
                    op = op_mret_t {};
                    break;
                default:
                    break;
                }
            }
        }
        else if (funct3 == 1)
        {
            op = op_csrrw_t { rd, rs1, csr };
        }
        else if (funct3 == 2)
        {
            op = op_csrrs_t { rd, rs1, csr };
        }
        else if (funct3 == 3)
        {
            op = op_csrrc_t { rd, rs1, csr };
        }
        else if (funct3 == 5)
        {
            op = op_csrrwi_t { rd, csr, zimm };
        }
        else if (funct3 == 6)
        {
            op = op_csrrsi_t { rd, csr, zimm };
        }
        else if (funct3 == 7)
        {
            op = op_csrrci_t { rd, csr, zimm };
        }
        break;
    default:
        break;
    }

    return op;
}

const op_t DecoderImpl::DecodeRV32M(int32_t insn) const
{
    const auto funct3 = Pick(insn, 12, 3);

    const auto rd = Pick(insn, 7, 5);
    const auto rs1 = Pick(insn, 15, 5);
    const auto rs2 = Pick(insn, 20, 5);

    op_t op;

    switch (funct3)
    {
    case 0:
        op = op_mul_t { rd, rs1, rs2 };
        break;
    case 1:
        op = op_mulh_t { rd, rs1, rs2 };
        break;
    case 2:
        op = op_mulhsu_t { rd, rs1, rs2 };
        break;
    case 3:
        op = op_mulhu_t { rd, rs1, rs2 };
        break;
    case 4:
        op = op_div_t { rd, rs1, rs2 };
        break;
    case 5:
        op = op_divu_t { rd, rs1, rs2 };
        break;
    case 6:
        op = op_rem_t { rd, rs1, rs2 };
        break;
    case 7:
        op = op_remu_t { rd, rs1, rs2 };
        break;
    default:
        break;
    }

    return op;
}

const op_t DecoderImpl::DecodeRV32A(int32_t insn) const
{
    const auto funct5 = Pick(insn, 27, 5);
    const bool aq = Pick(insn, 26, 1);
    const bool rl = Pick(insn, 25, 1);

    const auto rd = Pick(insn, 7, 5);
    const auto rs1 = Pick(insn, 15, 5);
    const auto rs2 = Pick(insn, 20, 5);

    op_t op;

    switch (funct5)
    {
    case 0b00010:
        if (rs2 == 0b00000)
        {
            op = op_lrw_t { rd, rs1, aq, rl };
        }
        break;
    case 0b00011:
        op = op_scw_t { rd, rs1, rs2, aq, rl };
        break;
    case 0b00001:
        op = op_amoswapw_t { rd, rs1, rs2, aq, rl };
        break;
    case 0b00000:
        op = op_amoaddw_t { rd, rs1, rs2, aq, rl };
        break;
    case 0b00100:
        op = op_amoxorw_t { rd, rs1, rs2, aq, rl };
        break;
    case 0b01100:
        op = op_amoandw_t { rd, rs1, rs2, aq, rl };
        break;
    case 0b01000:
        op = op_amoorw_t { rd, rs1, rs2, aq, rl };
        break;
    case 0b10000:
        op = op_amominw_t { rd, rs1, rs2, aq, rl };
        break;
    case 0b10100:
        op = op_amomaxw_t { rd, rs1, rs2, aq, rl };
        break;
    case 0b11000:
        op = op_amominuw_t { rd, rs1, rs2, aq, rl };
        break;
    case 0b11100:
        op = op_amomaxuw_t { rd, rs1, rs2, aq, rl };
        break;
    default:
        break;
    }

    return op;
}

}
