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

#include <cstdio>
#include <cstring>
#include <variant>

#include <rvtrace/common/Op.h>

namespace rvtrace {

namespace {

const char* IntRegNames[32] = {
    "x0",
    "x1",
    "x2",
    "x3",
    "x4",
    "x5",
    "x6",
    "x7",
    "x8",
    "x9",
    "x10",
    "x11",
    "x12",
    "x13",
    "x14",
    "x15",
    "x16",
    "x17",
    "x18",
    "x19",
    "x20",
    "x21",
    "x22",
    "x23",
    "x24",
    "x25",
    "x26",
    "x27",
    "x28",
    "x29",
    "x30",
    "x31",
};

struct StringGetter
{
    char buffer[64];

    const std::string operator()(const op_lui_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "lui %s,0x%x", IntRegNames[op.rd], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_auipc_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "auipc   %s,0x%x", IntRegNames[op.rd], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_jal_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "jal %s,0x%x", IntRegNames[op.rd], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_jalr_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "jalr    %s,0x%x", IntRegNames[op.rd], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_beq_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "beq %s,%s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_bne_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "bne %s,%s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_blt_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "blt %s,%s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_bge_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "bge %s,%s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_bltu_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "bltu    %s,%s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_bgeu_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "bgeu    %s,%s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_lb_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "lb  %s,%d(%s),", IntRegNames[op.rd], op.imm, IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_lh_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "lh   %s,%d(%s),", IntRegNames[op.rd], op.imm, IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_lw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "lw  %s,%d(%s),", IntRegNames[op.rd], op.imm, IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_lbu_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "lbu %s,%d(%s),", IntRegNames[op.rd], op.imm, IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_lhu_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "lhu %s,%d(%s),", IntRegNames[op.rd], op.imm, IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_sb_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "sb  %s,%d(%s)", IntRegNames[op.rs2], op.imm, IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_sh_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "sh  %s,%d(%s)", IntRegNames[op.rs2], op.imm, IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_sw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "sw  %s,%d(%s)", IntRegNames[op.rs2], op.imm, IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_addi_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "addi    %s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_slti_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "slti    %s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_sltiu_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "sltiu   %s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_xori_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "xori    %s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_ori_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "ori %s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_andi_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "andi    %s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], op.imm);
        return std::string(buffer);
    }

    const std::string operator()(const op_slli_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "slli    %s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], op.shamt);
        return std::string(buffer);
    }

    const std::string operator()(const op_srli_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "srli    %s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], op.shamt);
        return std::string(buffer);
    }

    const std::string operator()(const op_srai_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "srai    %s,%s,0x%x", IntRegNames[op.rd], IntRegNames[op.rs1], op.shamt);
        return std::string(buffer);
    }

    const std::string operator()(const op_add_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "add %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_sub_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "sub %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_sll_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "sll %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_slt_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "slt %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_sltu_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "sltu    %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_xor_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "xor %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_srl_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "srl %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_sra_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "sra %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_or_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "or  %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_and_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "and %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_fence_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "fence   0x%x,0x%x", op.pred, op.succ);
        return std::string(buffer);
    }

    const std::string operator()(const op_fencei_t&)
    {
        return std::string("fence.i");
    }

    const std::string operator()(const op_ecall_t&)
    {
        return std::string("ecall");
    }

    const std::string operator()(const op_ebreak_t&)
    {
        return std::string("ebreak");
    }

    const std::string operator()(const op_csrrw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "csrrw   %s,%s,%s", IntRegNames[op.rd], GetString(op.csr), IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_csrrs_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "csrrs   %s,%s,%s", IntRegNames[op.rd], GetString(op.csr), IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_csrrc_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "csrrc   %s,%s,%s", IntRegNames[op.rd], GetString(op.csr), IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_csrrwi_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "csrrwi  %s,%s,0x%x", IntRegNames[op.rd], GetString(op.csr), op.zimm);
        return std::string(buffer);
    }

    const std::string operator()(const op_csrrsi_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "csrrsi  %s,%s,0x%x", IntRegNames[op.rd], GetString(op.csr), op.zimm);
        return std::string(buffer);
    }

    const std::string operator()(const op_csrrci_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "csrrci  %s,%s,0x%x", IntRegNames[op.rd], GetString(op.csr), op.zimm);
        return std::string(buffer);
    }

    const std::string operator()(const op_uret_t&)
    {
        return std::string("uret");
    }

    const std::string operator()(const op_sret_t&)
    {
        return std::string("sret");
    }

    const std::string operator()(const op_mret_t&)
    {
        return std::string("mret");
    }

    const std::string operator()(const op_wfi_t&)
    {
        return std::string("wfi");
    }

    const std::string operator()(const op_sfencevma_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "sfence.vma  %s,%s", IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_mul_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "mul %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_mulh_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "mulh    %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_mulhsu_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "mulhsu  %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_mulhu_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "mulhu   %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_div_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "div %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_divu_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "divu    %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_rem_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "rem     %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_remu_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "remu    %s,%s,%s", IntRegNames[op.rd], IntRegNames[op.rs1], IntRegNames[op.rs2]);
        return std::string(buffer);
    }

    const std::string operator()(const op_lrw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "lr.w    %s,(%s)", IntRegNames[op.rd], IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_scw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "sc.w    %s,%s,(%s)", IntRegNames[op.rd], IntRegNames[op.rs2], IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_amoswapw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "amoswap.w   %s,%s,(%s)", IntRegNames[op.rd], IntRegNames[op.rs2], IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_amoaddw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "amoadd.w    %s,%s,(%s)", IntRegNames[op.rd], IntRegNames[op.rs2], IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_amoxorw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "amoxor.w    %s,%s,(%s)", IntRegNames[op.rd], IntRegNames[op.rs2], IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_amoandw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "amoand.w    %s,%s,(%s)", IntRegNames[op.rd], IntRegNames[op.rs2], IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_amoorw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "amoor.w %s,%s,(%s)", IntRegNames[op.rd], IntRegNames[op.rs2], IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_amominw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "amomin.w    %s,%s,(%s)", IntRegNames[op.rd], IntRegNames[op.rs2], IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_amomaxw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "amomax.w    %s,%s,(%s)", IntRegNames[op.rd], IntRegNames[op.rs2], IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_amominuw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "amominu.w   %s,%s,(%s)", IntRegNames[op.rd], IntRegNames[op.rs2], IntRegNames[op.rs1]);
        return std::string(buffer);
    }

    const std::string operator()(const op_amomaxuw_t& op)
    {
        std::snprintf(buffer, sizeof(buffer), "amomaxu.w   %s,%s,(%s)", IntRegNames[op.rd], IntRegNames[op.rs2], IntRegNames[op.rs1]);
        return std::string(buffer);
    }
};


}

const std::string GetString(const op_t& op)
{
    return std::visit(StringGetter(), op);
}

}
