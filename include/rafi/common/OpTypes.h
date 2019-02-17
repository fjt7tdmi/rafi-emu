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

#include "RvCsr.h"
#include "RvTypes.h"

namespace rafi {

enum class OpClass
{
    RV32I,
    RV32M,
    RV32A,
    RV32F,
    RV32D,
    RV32C,
    RV64I,
    RV64M,
    RV64A,
    RV64F,
    RV64D,
    RV64C,
};

enum class OpCode
{
    // Default
    unknown,

    // RV32I / RV64I
    lui,
    auipc,
    jal,
    jalr,
    beq,
    bne,
    blt,
    bge,
    bltu,
    bgeu,
    lb,
    lh,
    lw,
    lbu,
    lhu,
    lwu,
    ld,
    sb,
    sh,
    sw,
    sd,
    addi,
    addiw,
    slti,
    sltiu,
    xori,
    ori,
    andi,
    slli,
    slliw,
    srli,
    srliw,
    srai,
    sraiw,
    add,
    addw,
    sub,
    subw,
    sll,
    sllw,
    slt,
    sltu,
    xor_,
    srl,
    srlw,
    sra,
    sraw,
    or_,
    and_,
    fence,
    fence_i,
    ecall,
    ebreak,
    csrrw,
    csrrs,
    csrrc,
    csrrwi,
    csrrsi,
    csrrci,
    mret,
    sret,
    uret,
    wfi,
    sfence_vma,

    // RV32M
    mul,
    mulw,
    mulh,
    mulhsu,
    mulhu,
    div,
    divw,
    divu,
    divuw,
    rem,
    remw,
    remu,
    remuw,

    // RV32A
    lr_w,
    lr_d,
    sc_w,
    sc_d,
    amoswap_w,
    amoswap_d,
    amoadd_w,
    amoadd_d,
    amoxor_w,
    amoxor_d,
    amoand_w,
    amoand_d,
    amoor_w,
    amoor_d,
    amomin_w,
    amomin_d,
    amomax_w,
    amomax_d,
    amominu_w,
    amominu_d,
    amomaxu_w,
    amomaxu_d,

    // RV32F
    flw,
    fsw,
    fmadd_s,
    fmsub_s,
    fnmsub_s,
    fnmadd_s,
    fadd_s,
    fsub_s,
    fmul_s,
    fdiv_s,
    fsqrt_s,
    fsgnj_s,
    fsgnjn_s,
    fsgnjx_s,
    fmin_s,
    fmax_s,
    fcvt_w_s,
    fcvt_wu_s,
    fmv_x_w,
    feq_s,
    flt_s,
    fle_s,
    fclass_s,
    fcvt_l_s,
    fcvt_lu_s,
    fcvt_s_w,
    fcvt_s_l,
    fcvt_s_wu,
    fcvt_s_lu,
    fmv_w_x,

    // RV32D
    fld,
    fsd,
    fmadd_d,
    fmsub_d,
    fnmsub_d,
    fnmadd_d,
    fadd_d,
    fsub_d,
    fmul_d,
    fdiv_d,
    fsqrt_d,
    fsgnj_d,
    fsgnjn_d,
    fsgnjx_d,
    fmin_d,
    fmax_d,
    fmv_d_x,
    fmv_x_d,
    feq_d,
    flt_d,
    fle_d,
    fclass_d,
    fcvt_s_d,
    fcvt_w_d,
    fcvt_wu_d,
    fcvt_l_d,
    fcvt_lu_d,
    fcvt_d_s,
    fcvt_d_w,
    fcvt_d_wu,
    fcvt_d_l,
    fcvt_d_lu,

    // RVC
    c_addi4spn,
    c_fld,
    c_lq,
    c_lw,
    c_flw,
    c_ld,
    c_fsd,
    c_sq,
    c_sw,
    c_fsw,
    c_sd,
    c_nop,
    c_addi,
    c_jal,
    c_addiw,
    c_li,
    c_addi16sp,
    c_lui,
    c_srli,
    c_srli64,
    c_srai,
    c_srai64,
    c_andi,
    c_sub,
    c_xor,
    c_or,
    c_and,
    c_subw,
    c_addw,
    c_j,
    c_beqz,
    c_bnez,
    c_slli,
    c_slli64,
    c_fldsp,
    c_lqsp,
    c_lwsp,
    c_flwsp,
    c_ldsp,
    c_jr,
    c_mv,
    c_ebreak,
    c_jalr,
    c_add,
    c_fsdsp,
    c_sqsp,
    c_swsp,
    c_fswsp,
    c_sdsp,
};

struct OperandR
{
    int rd;
    int rs1;
    int rs2;
    int funct3;
    int funct7;
};

struct OperandR4
{
    int rd;
    int rs1;
    int rs2;
    int rs3;
    int funct3;
    int funct2;
};

struct OperandI
{
    int32_t imm;
    int rd;
    int rs1;
    int funct3;
};

struct OperandS
{
    int32_t imm;
    int rs1;
    int rs2;
    int funct3;
};

struct OperandB
{
    int32_t imm;
    int rs1;
    int rs2;
    int funct3;
};

struct OperandU
{
    int32_t imm;
    int rd;
};

struct OperandJ
{
    int32_t imm;
    int rd;
};

struct OperandShiftImm
{
    int rd;
    int rs1;
    int shamt;
};

struct OperandCsr
{
    int rd;
    int rs1;
    csr_addr_t csr;
};

struct OperandCsrImm
{
    int32_t zimm;
    int rd;
    csr_addr_t csr;
};

struct OperandFence
{
    int pred;
    int succ;
};

struct OperandCR
{
    int rd;
    int rs1;
    int rs2;
};

struct OperandCI
{
    int32_t imm;
    int rd;
    int rs1;
};

struct OperandCSS
{
    int32_t imm;
    int rs2;
};

struct OperandCIW
{
    int32_t imm;
    int rd;
};

struct OperandCL
{
    int32_t imm;
    int rd;
    int rs1;
};

struct OperandCS
{
    int32_t imm;
    int rs1;
    int rs2;
};

struct OperandCB
{
    int32_t imm;
    int rs1;
};

struct OperandCJ
{
    int32_t imm;
};

struct OperandNone
{
};

using Operand = std::variant<
    OperandR,
    OperandR4,
    OperandI,
    OperandS,
    OperandB,
    OperandU,
    OperandJ,
    OperandShiftImm,
    OperandCsr,
    OperandCsrImm,
    OperandFence,
    OperandCR,
    OperandCI,
    OperandCSS,
    OperandCIW,
    OperandCL,
    OperandCS,
    OperandCB,
    OperandCJ,
    OperandNone
>;

struct Op
{
    OpClass opClass;
    OpCode opCode;
    Operand operand;
};

}
