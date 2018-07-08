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

struct Op
{
    OpClass opClass;
    OpCode opCode;
    int rd;
    int rs1;
    int rs2;
    int32_t imm;
	int32_t zimm;
	int32_t shamt;
    int pred;
    int succ;
    int csr;
    bool aq;
    bool rl;
};

class OpDecoder
{
public:
    void Decode(Op* out, int32_t insn) const;
    void DumpOpCode(std::fstream& file, const Op& op) const;
	void DumpOperand(std::fstream& file, const Op& op) const;

private:
    void DecodeRV32I(Op* out, int32_t insn) const;
    void DecodeRV32M(Op* out, int32_t insn) const;
    void DecodeRV32A(Op* out, int32_t insn) const;
};

const char* GetString(OpCode opCode);
