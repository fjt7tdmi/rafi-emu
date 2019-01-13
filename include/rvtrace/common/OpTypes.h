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

enum class OpClass
{
    RV32I,
    RV32M,
    RV32A,
    RV32F,
    RV32D,
};

enum class OpCode
{
    // Default
    unknown,

    // RV32I
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
    sb,
    sh,
    sw,
    addi,
    slti,
    sltiu,
    xori,
    ori,
    andi,
    slli,
    srli,
    srai,
    add,
    sub,
    sll,
    slt,
    sltu,
    xor_,
    srl,
    sra,
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
    mulh,
    mulhsu,
    mulhu,
    div,
    divu,
    rem,
    remu,

    // RV32A
    lr_w,
    sc_w,
    amoswap_w,
    amoadd_w,
    amoxor_w,
    amoand_w,
    amoor_w,
    amomin_w,
    amomax_w,
    amominu_w,
    amomaxu_w,

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
    fcvt_s_w,
    fcvt_s_wu,
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
    fcvt_s_d,
    fcvt_d_s,
    feq_d,
    flt_d,
    fle_d,
    fclass_d,
    fcvt_w_d,
    fcvt_wu_d,
    fcvt_d_w,
    fcvt_d_wu,
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
    OperandNone
>;

struct Op
{
    OpClass opClass;
    OpCode opCode;
    Operand operand;
};

}