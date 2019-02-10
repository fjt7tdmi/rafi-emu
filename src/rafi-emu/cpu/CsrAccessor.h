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

#include <cstring>

#include <rafi/emu.h>

#include "Csr.h"

namespace rafi { namespace emu { namespace cpu {

class CsrAccessor
{
public:
    explicit CsrAccessor(Csr* pCsr)
        : m_pCsr(pCsr)
	{
        std::memset(&m_ReadEvent, 0, sizeof(m_ReadEvent));
        std::memset(&m_WriteEvent, 0, sizeof(m_WriteEvent));
	}

    uint32_t Read(int addr);
    void Write(int addr, uint32_t value);

    void ClearEvent();
    void CopyReadEvent(CsrReadEvent* pOut) const;
    void CopyWriteEvent(CsrWriteEvent* pOut) const;
    bool IsReadEventExist() const;
    bool IsWriteEventExist() const;

private:
    Csr* m_pCsr;

    bool m_ReadEventValid { false };
    bool m_WriteEventValid { false };

    CsrReadEvent m_ReadEvent;
    CsrWriteEvent m_WriteEvent;
};

}}}
