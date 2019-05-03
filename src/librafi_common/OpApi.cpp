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

#include <rafi/common.h>

namespace rafi {

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

}

const char* GetString(const OpClass& opClass)
{
#define GET_OP_NAME_CASE(arg) case OpClass::arg: return #arg
    switch (opClass)
    {
        GET_OP_NAME_CASE(RV32I);
        GET_OP_NAME_CASE(RV32A);
        GET_OP_NAME_CASE(RV32M);
    default:
        return "unknown";
    }
#undef GET_OP_NAME_CASE
}

const char* GetString(const OpCode& opCode)
{
#define GET_OP_NAME_CASE(arg) case OpCode::arg: return #arg
    switch (opCode)
    {
        // RV32I
        GET_OP_NAME_CASE(lui);
        GET_OP_NAME_CASE(auipc);
        GET_OP_NAME_CASE(jal);
        GET_OP_NAME_CASE(jalr);
        GET_OP_NAME_CASE(beq);
        GET_OP_NAME_CASE(bne);
        GET_OP_NAME_CASE(blt);
        GET_OP_NAME_CASE(bge);
        GET_OP_NAME_CASE(bltu);
        GET_OP_NAME_CASE(bgeu);
        GET_OP_NAME_CASE(lb);
        GET_OP_NAME_CASE(lh);
        GET_OP_NAME_CASE(lw);
        GET_OP_NAME_CASE(lbu);
        GET_OP_NAME_CASE(lhu);
        GET_OP_NAME_CASE(sb);
        GET_OP_NAME_CASE(sh);
        GET_OP_NAME_CASE(sw);
        GET_OP_NAME_CASE(addi);
        GET_OP_NAME_CASE(slti);
        GET_OP_NAME_CASE(sltiu);
        GET_OP_NAME_CASE(xori);
        GET_OP_NAME_CASE(ori);
        GET_OP_NAME_CASE(andi);
        GET_OP_NAME_CASE(slli);
        GET_OP_NAME_CASE(srli);
        GET_OP_NAME_CASE(srai);
        GET_OP_NAME_CASE(add);
        GET_OP_NAME_CASE(sub);
        GET_OP_NAME_CASE(sll);
        GET_OP_NAME_CASE(slt);
        GET_OP_NAME_CASE(sltu);
        GET_OP_NAME_CASE(xor_);
        GET_OP_NAME_CASE(srl);
        GET_OP_NAME_CASE(sra);
        GET_OP_NAME_CASE(or_);
        GET_OP_NAME_CASE(and_);
        GET_OP_NAME_CASE(fence);
        GET_OP_NAME_CASE(fence_i);
        GET_OP_NAME_CASE(ecall);
        GET_OP_NAME_CASE(ebreak);
        GET_OP_NAME_CASE(csrrw);
        GET_OP_NAME_CASE(csrrs);
        GET_OP_NAME_CASE(csrrc);
        GET_OP_NAME_CASE(csrrwi);
        GET_OP_NAME_CASE(csrrsi);
        GET_OP_NAME_CASE(csrrci);
        GET_OP_NAME_CASE(mret);
        GET_OP_NAME_CASE(sret);
        GET_OP_NAME_CASE(uret);
        GET_OP_NAME_CASE(wfi);
        GET_OP_NAME_CASE(sfence_vma);

        // RV32M
        GET_OP_NAME_CASE(mul);
        GET_OP_NAME_CASE(mulh);
        GET_OP_NAME_CASE(mulhsu);
        GET_OP_NAME_CASE(mulhu);
        GET_OP_NAME_CASE(div);
        GET_OP_NAME_CASE(divu);
        GET_OP_NAME_CASE(rem);
        GET_OP_NAME_CASE(remu);

        // RV32A
        GET_OP_NAME_CASE(lr_w);
        GET_OP_NAME_CASE(sc_w);
        GET_OP_NAME_CASE(amoswap_w);
        GET_OP_NAME_CASE(amoadd_w);
        GET_OP_NAME_CASE(amoxor_w);
        GET_OP_NAME_CASE(amoand_w);
        GET_OP_NAME_CASE(amoor_w);
        GET_OP_NAME_CASE(amomin_w);
        GET_OP_NAME_CASE(amomax_w);
        GET_OP_NAME_CASE(amominu_w);
        GET_OP_NAME_CASE(amomaxu_w);

        // RV32F
        GET_OP_NAME_CASE(flw);
        GET_OP_NAME_CASE(fsw);
        GET_OP_NAME_CASE(fmadd_s);
        GET_OP_NAME_CASE(fmsub_s);
        GET_OP_NAME_CASE(fnmsub_s);
        GET_OP_NAME_CASE(fnmadd_s);
        GET_OP_NAME_CASE(fadd_s);
        GET_OP_NAME_CASE(fsub_s);
        GET_OP_NAME_CASE(fmul_s);
        GET_OP_NAME_CASE(fdiv_s);
        GET_OP_NAME_CASE(fsqrt_s);
        GET_OP_NAME_CASE(fsgnj_s);
        GET_OP_NAME_CASE(fsgnjn_s);
        GET_OP_NAME_CASE(fsgnjx_s);
        GET_OP_NAME_CASE(fmin_s);
        GET_OP_NAME_CASE(fmax_s);
        GET_OP_NAME_CASE(fcvt_w_s);
        GET_OP_NAME_CASE(fcvt_wu_s);
        GET_OP_NAME_CASE(fmv_x_w);
        GET_OP_NAME_CASE(feq_s);
        GET_OP_NAME_CASE(flt_s);
        GET_OP_NAME_CASE(fle_s);
        GET_OP_NAME_CASE(fclass_s);
        GET_OP_NAME_CASE(fcvt_s_w);
        GET_OP_NAME_CASE(fcvt_s_wu);
        GET_OP_NAME_CASE(fmv_w_x);

        // RV32D
        GET_OP_NAME_CASE(fld);
        GET_OP_NAME_CASE(fsd);
        GET_OP_NAME_CASE(fmadd_d);
        GET_OP_NAME_CASE(fmsub_d);
        GET_OP_NAME_CASE(fnmsub_d);
        GET_OP_NAME_CASE(fnmadd_d);
        GET_OP_NAME_CASE(fadd_d);
        GET_OP_NAME_CASE(fsub_d);
        GET_OP_NAME_CASE(fmul_d);
        GET_OP_NAME_CASE(fdiv_d);
        GET_OP_NAME_CASE(fsqrt_d);
        GET_OP_NAME_CASE(fsgnj_d);
        GET_OP_NAME_CASE(fsgnjn_d);
        GET_OP_NAME_CASE(fsgnjx_d);
        GET_OP_NAME_CASE(fmin_d);
        GET_OP_NAME_CASE(fmax_d);
        GET_OP_NAME_CASE(fcvt_s_d);
        GET_OP_NAME_CASE(fcvt_d_s);
        GET_OP_NAME_CASE(feq_d);
        GET_OP_NAME_CASE(flt_d);
        GET_OP_NAME_CASE(fle_d);
        GET_OP_NAME_CASE(fclass_d);
        GET_OP_NAME_CASE(fcvt_w_d);
        GET_OP_NAME_CASE(fcvt_wu_d);
        GET_OP_NAME_CASE(fcvt_d_w);
        GET_OP_NAME_CASE(fcvt_d_wu);

        // RVC
        GET_OP_NAME_CASE(c_addi4spn);
        GET_OP_NAME_CASE(c_fld);
        GET_OP_NAME_CASE(c_lq);
        GET_OP_NAME_CASE(c_lw);
        GET_OP_NAME_CASE(c_flw);
        GET_OP_NAME_CASE(c_ld);
        GET_OP_NAME_CASE(c_fsd);
        GET_OP_NAME_CASE(c_sq);
        GET_OP_NAME_CASE(c_sw);
        GET_OP_NAME_CASE(c_fsw);
        GET_OP_NAME_CASE(c_sd);
        GET_OP_NAME_CASE(c_nop);
        GET_OP_NAME_CASE(c_addi);
        GET_OP_NAME_CASE(c_jal);
        GET_OP_NAME_CASE(c_addiw);
        GET_OP_NAME_CASE(c_li);
        GET_OP_NAME_CASE(c_addi16sp);
        GET_OP_NAME_CASE(c_lui);
        GET_OP_NAME_CASE(c_srli);
        GET_OP_NAME_CASE(c_srli64);
        GET_OP_NAME_CASE(c_srai);
        GET_OP_NAME_CASE(c_srai64);
        GET_OP_NAME_CASE(c_andi);
        GET_OP_NAME_CASE(c_sub);
        GET_OP_NAME_CASE(c_xor);
        GET_OP_NAME_CASE(c_or);
        GET_OP_NAME_CASE(c_and);
        GET_OP_NAME_CASE(c_subw);
        GET_OP_NAME_CASE(c_addw);
        GET_OP_NAME_CASE(c_j);
        GET_OP_NAME_CASE(c_beqz);
        GET_OP_NAME_CASE(c_bnez);
        GET_OP_NAME_CASE(c_slli);
        GET_OP_NAME_CASE(c_slli64);
        GET_OP_NAME_CASE(c_fldsp);
        GET_OP_NAME_CASE(c_lqsp);
        GET_OP_NAME_CASE(c_lwsp);
        GET_OP_NAME_CASE(c_flwsp);
        GET_OP_NAME_CASE(c_ldsp);
        GET_OP_NAME_CASE(c_jr);
        GET_OP_NAME_CASE(c_mv);
        GET_OP_NAME_CASE(c_ebreak);
        GET_OP_NAME_CASE(c_jalr);
        GET_OP_NAME_CASE(c_add);
        GET_OP_NAME_CASE(c_fsdsp);
        GET_OP_NAME_CASE(c_sqsp);
        GET_OP_NAME_CASE(c_swsp);
        GET_OP_NAME_CASE(c_fswsp);
        GET_OP_NAME_CASE(c_sdsp);
    default:
        return "unknown";
    }
#undef GET_OP_NAME_CASE
}

bool IsRV32(OpClass opClass)
{
    return
        opClass == OpClass::RV32I ||
        opClass == OpClass::RV32M ||
        opClass == OpClass::RV32A ||
        opClass == OpClass::RV32F ||
        opClass == OpClass::RV32D ||
        opClass == OpClass::RV32C;
}

bool IsRV64(OpClass opClass)
{
    return
        opClass == OpClass::RV64I ||
        opClass == OpClass::RV64M ||
        opClass == OpClass::RV64A ||
        opClass == OpClass::RV64F ||
        opClass == OpClass::RV64D ||
        opClass == OpClass::RV64C;
}

class SNPrintOpVisitor
{
public:
    SNPrintOpVisitor(char* buffer, int bufferSize, OpCode opCode)
        : m_Buffer(buffer)
        , m_BufferSize(bufferSize)
        , m_OpCode(opCode)
    {
    }

    int operator()(const OperandR& operand)
    {
        const char* opCode = GetString(m_OpCode);

        // TODO: Print int/fp reg names
        return std::snprintf(m_Buffer, m_BufferSize, "%s (rd:%d, rs1:%d, rs2:%d)", opCode, operand.rd, operand.rs1, operand.rs2);
    }

    int operator()(const OperandR4& operand)
    {
        const char* opCode = GetString(m_OpCode);

        // TODO: Print int/fp reg names
        return std::snprintf(m_Buffer, m_BufferSize, "%s (rd:%d, rs1:%d, rs2:%d, rs3:%d)", opCode, operand.rd, operand.rs1, operand.rs2, operand.rs3);
    }

    int operator()(const OperandI& operand)
    {
        // TODO: Print int/fp reg names
        return std::snprintf(m_Buffer, m_BufferSize, "%s (rd:%d, imm:0x%x)", GetString(m_OpCode), operand.rd, static_cast<uint32_t>(operand.imm));
    }

    int operator()(const OperandS& operand)
    {
        const char* opCode = GetString(m_OpCode);

        // TODO: Print int/fp reg names
        return std::snprintf(m_Buffer, m_BufferSize, "%s (rs1:%d, imm:0x%x, rs2:%d)", opCode, operand.rs1, static_cast<uint32_t>(operand.imm), operand.rs2);
    }

    int operator()(const OperandB& operand)
    {
        const char* opCode = GetString(m_OpCode);
        const char* rs1 = IntRegNames[operand.rs1];
        const char* rs2 = IntRegNames[operand.rs2];

        return std::snprintf(m_Buffer, m_BufferSize, "%s %s,%d(%s)", opCode, rs1, static_cast<int32_t>(operand.imm), rs2);
    }

    int operator()(const OperandU& operand)
    {
        const char* opCode = GetString(m_OpCode);
        const char* rd = IntRegNames[operand.rd];

        return std::snprintf(m_Buffer, m_BufferSize, "%s %s,0x%x", opCode, rd, static_cast<uint32_t>(operand.imm));
    }

    int operator()(const OperandJ& operand)
    {
        const char* opCode = GetString(m_OpCode);
        const char* rd = IntRegNames[operand.rd];

        return std::snprintf(m_Buffer, m_BufferSize, "%s %s,0x%x", opCode, rd, static_cast<uint32_t>(operand.imm));
    }

    int operator()(const OperandShiftImm& operand)
    {
        const char* opCode = GetString(m_OpCode);
        const char* rd = IntRegNames[operand.rd];
        const char* rs1 = IntRegNames[operand.rs1];

        return std::snprintf(m_Buffer, m_BufferSize, "%s %s,%s,0x%x", opCode, rd, rs1, operand.shamt);
    }

    int operator()(const OperandCsr& operand)
    {
        const char* opCode = GetString(m_OpCode);
        const char* rd = IntRegNames[operand.rd];
        const char* rs1 = IntRegNames[operand.rs1];

        return std::snprintf(m_Buffer, m_BufferSize, "%s %s,%s,%s", opCode, rd, GetString(operand.csr), rs1);
    }

    int operator()(const OperandCsrImm& operand)
    {
        const char* opCode = GetString(m_OpCode);
        const char* rd = IntRegNames[operand.rd];

        return std::snprintf(m_Buffer, m_BufferSize, "%s %s,%s,0x%x", opCode, rd, GetString(operand.csr), operand.zimm);
    }

    int operator()(const OperandFence& operand)
    {
        const char* opCode = GetString(m_OpCode);

        return std::snprintf(m_Buffer, m_BufferSize, "%s (pred:0x%x, succ:0x%x)", opCode, operand.pred, operand.succ);
    }

    int operator()(const OperandCR& operand)
    {
        const char* opCode = GetString(m_OpCode);

        // TODO: Print int/fp reg names
        return std::snprintf(m_Buffer, m_BufferSize, "%s (rd:%d, rs1:%d, rs2:%d)", opCode, operand.rd, operand.rs1, operand.rs2);
    }

    int operator()(const OperandCI& operand)
    {
        const char* opCode = GetString(m_OpCode);

        // TODO: Print int/fp reg names
        return std::snprintf(m_Buffer, m_BufferSize, "%s (rd:%d, rs1:%d, imm:0x%x)", opCode, operand.rd, operand.rs1, static_cast<int32_t>(operand.imm));
    }

    int operator()(const OperandCSS& operand)
    {
        const char* opCode = GetString(m_OpCode);

        // TODO: Print int/fp reg names
        return std::snprintf(m_Buffer, m_BufferSize, "%s (imm:0x%x, rs2:%d)", opCode, static_cast<uint32_t>(operand.imm), operand.rs2);
    }

    int operator()(const OperandCIW& operand)
    {
        const char* opCode = GetString(m_OpCode);

        // TODO: Print int/fp reg names
        return std::snprintf(m_Buffer, m_BufferSize, "%s (rd:%d, imm:0x%x)", opCode, operand.rd, static_cast<uint32_t>(operand.imm));
    }

    int operator()(const OperandCL& operand)
    {
        const char* opCode = GetString(m_OpCode);

        // TODO: Print int/fp reg names
        return std::snprintf(m_Buffer, m_BufferSize, "%s (rd:%d, rs1:%d, imm:0x%x)", opCode, operand.rd, operand.rs1, static_cast<uint32_t>(operand.imm));
    }

    int operator()(const OperandCS& operand)
    {
        const char* opCode = GetString(m_OpCode);

        // TODO: Print int/fp reg names
        return std::snprintf(m_Buffer, m_BufferSize, "%s (rs1:%d, imm:0x%x, rs2:%d)", opCode, operand.rs1, static_cast<uint32_t>(operand.imm), operand.rs2);
    }

    int operator()(const OperandCB& operand)
    {
        const char* opCode = GetString(m_OpCode);
        const char* rs1 = IntRegNames[operand.rs1];

        return std::snprintf(m_Buffer, m_BufferSize, "%s %s,%d", opCode, rs1, static_cast<int32_t>(operand.imm));
    }

    int operator()(const OperandCJ& operand)
    {
        const char* opCode = GetString(m_OpCode);

        return std::snprintf(m_Buffer, m_BufferSize, "%s %d", opCode, static_cast<int32_t>(operand.imm));
    }

    int operator()(const OperandNone&)
    {
        const char* opCode = GetString(m_OpCode);

        return std::snprintf(m_Buffer, m_BufferSize, "%s", opCode);
    }

private:
    char* m_Buffer;
    int m_BufferSize;
    OpCode m_OpCode;
};

int SNPrintOp(char* buffer, int bufferSize, const Op& op)
{
    return std::visit(SNPrintOpVisitor(buffer, bufferSize, op.opCode), op.operand);
}

}
