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

#include <iostream>
#include <variant>

#include "RvTypes.h"

namespace rvtrace {

// ----------------------------------------------------------------------------
// RV32I

struct op_lui_t
{
    int rd;
    int32_t imm;
};

struct op_auipc_t
{
    int rd;
    int32_t imm;
};

struct op_jal_t
{
    int rd;
    int32_t imm;
};

struct op_jalr_t
{
    int rd;
    int rs1;
    int32_t imm;
};

struct op_beq_t
{
    int rd;
    int rs1;
    int rs2;
    int32_t imm;
};

struct op_bne_t
{
    int rd;
    int rs1;
    int rs2;
    int32_t imm;
};

struct op_blt_t
{
    int rd;
    int rs1;
    int rs2;
    int32_t imm;
};

struct op_bge_t
{
    int rd;
    int rs1;
    int rs2;
    int32_t imm;
};

struct op_bltu_t
{
    int rd;
    int rs1;
    int rs2;
    int32_t imm;
};

struct op_bgeu_t
{
    int rd;
    int rs1;
    int rs2;
    int32_t imm;
};

struct op_lb_t
{
    int rd;
    int rs1;
    int32_t imm;
};

struct op_lh_t
{
    int rd;
    int rs1;
    int32_t imm;
};

struct op_lw_t
{
    int rd;
    int rs1;
    int32_t imm;
};

struct op_lbu_t
{
    int rd;
    int rs1;
    int32_t imm;
};

struct op_lhu_t
{
    int rd;
    int rs1;
    int32_t imm;
};

struct op_sb_t
{
    int rd;
    int rs1;
    int rs2;
    int32_t imm;
};

struct op_sh_t
{
    int rd;
    int rs1;
    int rs2;
    int32_t imm;
};

struct op_sw_t
{
    int rd;
    int rs1;
    int rs2;
    int32_t imm;
};

struct op_addi_t
{
    int rd;
    int rs1;
    int32_t imm;
};

struct op_slti_t
{
    int rd;
    int rs1;
    int32_t imm;
};

struct op_sltiu_t
{
    int rd;
    int rs1;
    int32_t imm;
};

struct op_xori_t
{
    int rd;
    int rs1;
    int32_t imm;
};

struct op_ori_t
{
    int rd;
    int rs1;
    int32_t imm;
};

struct op_andi_t
{
    int rd;
    int rs1;
    int32_t imm;
};

struct op_slli_t
{
    int rd;
    int rs1;
    int shamt;
};

struct op_srli_t
{
    int rd;
    int rs1;
    int shamt;
};

struct op_srai_t
{
    int rd;
    int rs1;
    int shamt;
};

struct op_add_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_sub_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_sll_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_slt_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_sltu_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_xor_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_srl_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_sra_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_or_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_and_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_fence_t
{
    int pred;
    int succ;
};

struct op_fencei_t
{
};

struct op_ecall_t
{
};

struct op_ebreak_t
{
};

struct op_csrrw_t
{
    int rd;
    int rs1;
    csr_addr_t csr;
};

struct op_csrrs_t
{
    int rd;
    int rs1;
    csr_addr_t csr;
};

struct op_csrrc_t
{
    int rd;
    int rs1;
    csr_addr_t csr;
};

struct op_csrrwi_t
{
    int rd;
    csr_addr_t csr;
    int32_t zimm;
};

struct op_csrrsi_t
{
    int rd;
    csr_addr_t csr;
    int32_t zimm;
};

struct op_csrrci_t
{
    int rd;
    csr_addr_t csr;
    int32_t zimm;
};

// ----------------------------------------------------------------------------
// RV32I (privileged)

struct op_uret_t
{
};

struct op_sret_t
{
};

struct op_mret_t
{
};

struct op_wfi_t
{
};

struct op_sfencevma_t
{
    int rs1;
    int rs2;
};

// ----------------------------------------------------------------------------
// RV32M

struct op_mul_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_mulh_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_mulhsu_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_mulhu_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_div_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_divu_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_rem_t
{
    int rd;
    int rs1;
    int rs2;
};

struct op_remu_t
{
    int rd;
    int rs1;
    int rs2;
};

// ----------------------------------------------------------------------------
// RV32A

struct op_lrw_t
{
    int rd;
    int rs1;
    bool aq;
    bool rl;
};

struct op_scw_t
{
    int rd;
    int rs1;
    int rs2;
    bool aq;
    bool rl;
};

struct op_amoswapw_t
{
    int rd;
    int rs1;
    int rs2;
    bool aq;
    bool rl;
};

struct op_amoaddw_t
{
    int rd;
    int rs1;
    int rs2;
    bool aq;
    bool rl;
};

struct op_amoxorw_t
{
    int rd;
    int rs1;
    int rs2;
    bool aq;
    bool rl;
};

struct op_amoandw_t
{
    int rd;
    int rs1;
    int rs2;
    bool aq;
    bool rl;
};

struct op_amoorw_t
{
    int rd;
    int rs1;
    int rs2;
    bool aq;
    bool rl;
};

struct op_amominw_t
{
    int rd;
    int rs1;
    int rs2;
    bool aq;
    bool rl;
};

struct op_amomaxw_t
{
    int rd;
    int rs1;
    int rs2;
    bool aq;
    bool rl;
};

struct op_amominuw_t
{
    int rd;
    int rs1;
    int rs2;
    bool aq;
    bool rl;
};

struct op_amomaxuw_t
{
    int rd;
    int rs1;
    int rs2;
    bool aq;
    bool rl;
};

// ----------------------------------------------------------------------------
// op_t

using op_t = std::variant<
    op_lui_t,
    op_auipc_t,
    op_jal_t,
    op_jalr_t,
    op_beq_t,
    op_bne_t,
    op_blt_t,
    op_bge_t,
    op_bltu_t,
    op_bgeu_t,
    op_lb_t,
    op_lh_t,
    op_lw_t,
    op_lbu_t,
    op_lhu_t,
    op_sb_t,
    op_sh_t,
    op_sw_t,
    op_addi_t,
    op_slti_t,
    op_sltiu_t,
    op_xori_t,
    op_ori_t,
    op_andi_t,
    op_slli_t,
    op_srli_t,
    op_srai_t,
    op_add_t,
    op_sub_t,
    op_sll_t,
    op_slt_t,
    op_sltu_t,
    op_xor_t,
    op_srl_t,
    op_sra_t,
    op_or_t,
    op_and_t,
    op_fence_t,
    op_fencei_t,
    op_ecall_t,
    op_ebreak_t,
    op_csrrw_t,
    op_csrrs_t,
    op_csrrc_t,
    op_csrrwi_t,
    op_csrrsi_t,
    op_csrrci_t,
    op_uret_t,
    op_sret_t,
    op_mret_t,
    op_wfi_t,
    op_sfencevma_t,
    op_mul_t,
    op_mulh_t,
    op_mulhsu_t,
    op_mulhu_t,
    op_div_t,
    op_divu_t,
    op_rem_t,
    op_remu_t,
    op_lrw_t,
    op_scw_t,
    op_amoswapw_t,
    op_amoaddw_t,
    op_amoxorw_t,
    op_amoandw_t,
    op_amoorw_t,
    op_amominw_t,
    op_amomaxw_t,
    op_amominuw_t,
    op_amomaxuw_t
>;

const std::string GetString(const op_t& op);

}