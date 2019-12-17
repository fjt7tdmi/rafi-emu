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

#ifdef WIN32

#define NOMINMAX
#include <Windows.h>
#include <Winsock.h>

#else // WIN32

#include <errno.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#endif // WIN32

#include <rafi/emu.h>

namespace rafi { namespace emu {

// ----------------------------------------------------------------------------
// Type definitions

#ifdef WIN32

using socklen_t = int;

#else // WIN32

using socklen_t = ::socklen_t;

#endif // WIN32

// ----------------------------------------------------------------------------
// Socket API control

void InitializeSocket();
void FinalizeSocket();

// ----------------------------------------------------------------------------
// Socket API wrapper

int close(int fd);

int GetSocketError();

}}
