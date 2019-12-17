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

#include <rafi/emu.h>

#include "Socket.h"

namespace rafi { namespace emu {

void InitializeSocket()
{
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,0), &wsaData);
#endif
}

void FinalizeSocket()
{
#ifdef WIN32
    WSACleanup();
#endif
}

int close(int fd)
{
#ifdef WIN32
    return ::closesocket(fd);
#else
    return ::close(fd);
#endif
}

int GetSocketError()
{
#ifdef WIN32
    return GetLastError();
#else
    return errno;
#endif
}

}}
