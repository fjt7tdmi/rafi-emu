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

#include "RvTypes.h"

namespace rvtrace {

const char* GetString(MemoryAccessType accessType);
const char* GetString(PrivilegeLevel level);
const char* GetString(TrapType trapType);
const char* GetString(ExceptionType exceptionType);
const char* GetString(InterruptType interruptType);
const char* GetString(csr_addr_t addr);
const char* GetString(csr_addr_t addr, const char* defaultValue);

const char* GetFpRegName(int index);

}
