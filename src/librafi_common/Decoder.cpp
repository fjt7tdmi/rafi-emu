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

#include <rafi/common.h>

#include "DecoderImpl.h"

namespace rafi {

Decoder::Decoder(XLEN xlen)
{
    m_pImpl = new DecoderImpl(xlen);
}

Decoder::~Decoder()
{
    delete m_pImpl;
}

Op Decoder::Decode(uint16_t insn) const
{
    return m_pImpl->Decode(insn);
}

Op Decoder::Decode(uint32_t insn) const
{
    return m_pImpl->Decode(insn);
}

bool Decoder::IsCompressedInstruction(uint16_t insn) const
{
    return m_pImpl->IsCompressedInstruction(insn);
}

bool Decoder::IsCompressedInstruction(uint32_t insn) const
{
    return m_pImpl->IsCompressedInstruction(insn);
}

}
