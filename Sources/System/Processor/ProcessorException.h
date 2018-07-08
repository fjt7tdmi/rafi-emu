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
#include <string>
#include <iostream>

#include "../../Common/RvTypes.h"

class ProcessorException
{
public:
    ProcessorException(ExceptionType cause, int32_t pc, int32_t trapValue)
        : m_Cause(cause)
        , m_ProgramCounter(pc)
        , m_TrapValue(trapValue)
    {
    }

    ExceptionType GetCause() const
    {
        return m_Cause;
    }

    int32_t GetProgramCounter() const
    {
        return m_ProgramCounter;
    }

    int32_t GetTrapValue() const
    {
        return m_TrapValue;
    }

private:
    ExceptionType m_Cause;
    int32_t m_ProgramCounter;
    int32_t m_TrapValue;
};

class InstructionAddressMisalignedException : public ProcessorException
{
public:
    explicit InstructionAddressMisalignedException(int32_t pc)
        : ProcessorException(ExceptionType::InstructionAddressMisaligned, pc, pc)
    {
    }
};

class InstructionAccessFaultException : public ProcessorException
{
public:
    explicit InstructionAccessFaultException(int32_t pc)
        : ProcessorException(ExceptionType::InstructionAccessFault, pc, pc)
    {
    }
};

class IllegalInstructionException : public ProcessorException
{
public:
    IllegalInstructionException(int32_t pc, int32_t insn)
        : ProcessorException(ExceptionType::IllegalInstruction, pc, insn)
    {
    }
};

class BreakpointException : public ProcessorException
{
public:
    explicit BreakpointException(int32_t pc)
        : ProcessorException(ExceptionType::Breakpoint, pc, pc)
    {
    }
};

class LoadAddressMisalignedException : public ProcessorException
{
public:
    LoadAddressMisalignedException(int32_t pc, int32_t address)
        : ProcessorException(ExceptionType::LoadAddressMisaligned, pc, address)
    {
    }
};

class LoadAccessFaultException : public ProcessorException
{
public:
    LoadAccessFaultException(int32_t pc, int32_t address)
        : ProcessorException(ExceptionType::LoadAccessFault, pc, address)
    {
    }
};

class StoreAccessFaultException : public ProcessorException
{
public:
    StoreAccessFaultException(int32_t pc, int32_t address)
        : ProcessorException(ExceptionType::StoreAccessFault, pc, address)
    {
    }
};

class EnvironmentCallFromUserException : public ProcessorException
{
public:
    explicit EnvironmentCallFromUserException(int32_t pc)
        : ProcessorException(ExceptionType::EnvironmentCallFromUser, pc, 0)
    {
    }
};

class EnvironmentCallFromSupervisorException : public ProcessorException
{
public:
    explicit EnvironmentCallFromSupervisorException(int32_t pc)
        : ProcessorException(ExceptionType::EnvironmentCallFromSupervisor, pc, 0)
    {
    }
};

class EnvironmentCallFromMachineException : public ProcessorException
{
public:
    explicit EnvironmentCallFromMachineException(int32_t pc)
        : ProcessorException(ExceptionType::EnvironmentCallFromMachine, pc, 0)
    {
    }
};

class InstructionPageFaultException : public ProcessorException
{
public:
    explicit InstructionPageFaultException(int32_t pc)
        : ProcessorException(ExceptionType::InstructionPageFault, pc, pc)
    {
    }
};

class LoadPageFaultException : public ProcessorException
{
public:
    LoadPageFaultException(int32_t pc, int32_t address)
        : ProcessorException(ExceptionType::LoadPageFault, pc, address)
    {
    }
};

class StorePageFaultException : public ProcessorException
{
public:
    StorePageFaultException(int32_t pc, int32_t address)
        : ProcessorException(ExceptionType::StorePageFault, pc, address)
    {
    }
};
