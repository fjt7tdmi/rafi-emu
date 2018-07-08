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

#include <cstdint>
#include <cstdio>

#include <iostream>

#include "TraceBinary.h"

TraceChildHeader* FindTraceChild(NodeType nodeType, char* pNode, size_t nodeSize);

BasicInfoNode* FindBasicInfoNode(char* pNode, size_t nodeSize);
IoNode* FindIoNode(char* pNode, size_t nodeSize);
Pc32Node* FindPc32Node(char* pNode, size_t nodeSize);
IntReg32Node* FindIntReg32Node(char* pNode, size_t nodeSize);
Csr32NodeHeader* FindCsr32Node(char* pNode, size_t nodeSize);
Trap32Node* FindTrap32Node(char* pNode, size_t nodeSize);
MemoryAccess32Node* FindMemoryAccess32Node(char* pNode, size_t nodeSize);
MemoryNodeHeader* FindMemoryNode(char* pNode, size_t nodeSize);
