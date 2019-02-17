cmake_minimum_required(VERSION 3.8)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()

find_package(Boost)

if (NOT ${Boost_FOUND})
    message(FATAL_ERROR "Cannot find boost.")
endif()

if (MSVC)
    if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(GoogleTest_LIBRARIES "gtestd" "gtest_maind")
        set(GoogleTest_INCLUDE_DIRS "third_party/googletest/googletest/include")
        set(GoogleTest_LIBRARY_DIRS "third_party/googletest/build/googletest/Debug" "third_party/googletest/build/googlemock/gtest/Debug")
    else()
        set(GoogleTest_LIBRARIES "gtest" "gtest_main")
        set(GoogleTest_INCLUDE_DIRS "third_party/googletest/googletest/include")
        set(GoogleTest_LIBRARY_DIRS "third_party/googletest/build/googletest/Release" "third_party/googletest/build/googlemock/gtest/Release")
    endif()
else()
    set(Boost_LIBRARIES "boost_program_options")
    set(FS_LIBRARIES "stdc++fs")

    set(GoogleTest_LIBRARIES "gtest" "gtest_main" "pthread")
    set(GoogleTest_INCLUDE_DIRS "third_party/googletest/googletest/include")
    set(GoogleTest_LIBRARY_DIRS "third_party/googletest/build/googletest")
endif()

set(Softfloat_INCLUDE_DIRS third_party/berkeley-softfloat-3/source/include third_party/berkeley-softfloat-3/source/RISCV src/librafi_fp/include)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GoogleTest_INCLUDE_DIRS})

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${GoogleTest_LIBRARY_DIRS})

add_library(librafi_common
    include/rafi/common.h
    include/rafi/common/BitField.h
    include/rafi/common/Decoder.h
    include/rafi/common/Macro.h
    include/rafi/common/OpApi.h
    include/rafi/common/OpTypes.h
    include/rafi/common/RvApi.h
    include/rafi/common/RvTypes.h
    src/librafi_common/Decoder.cpp
    src/librafi_common/DecoderImpl.cpp
    src/librafi_common/DecoderImpl.h
    src/librafi_common/OpApi.cpp
    src/librafi_common/RvApi.cpp
)

add_library(librafi_fp
    include/rafi/common.h
    include/rafi/fp.h
    include/rafi/common/BitField.h
    include/rafi/fp/FpApi.h
    include/rafi/fp/ScopedFpRound.h
    src/librafi_fp/include/platform.h
    src/librafi_fp/FpApi.cpp
    src/librafi_fp/ScopedFpRound.cpp

    # berkeley-softfloat-3 include
    third_party/berkeley-softfloat-3/source/include/internals.h
    third_party/berkeley-softfloat-3/source/include/primitives.h
    third_party/berkeley-softfloat-3/source/include/primitiveTypes.h
    third_party/berkeley-softfloat-3/source/include/softfloat_types.h
    third_party/berkeley-softfloat-3/source/include/softfloat.h

    # berkeley-softfloat-3 primitive
    third_party/berkeley-softfloat-3/source/s_compare96M.c
    third_party/berkeley-softfloat-3/source/s_compare128M.c
    third_party/berkeley-softfloat-3/source/s_shortShiftLeft64To96M.c
    third_party/berkeley-softfloat-3/source/s_shortShiftLeftM.c
    third_party/berkeley-softfloat-3/source/s_shiftLeftM.c
    third_party/berkeley-softfloat-3/source/s_shortShiftRightM.c
    third_party/berkeley-softfloat-3/source/s_shortShiftRightJam64.c
    third_party/berkeley-softfloat-3/source/s_shortShiftRightJamM.c
    third_party/berkeley-softfloat-3/source/s_shiftRightJam32.c
    third_party/berkeley-softfloat-3/source/s_shiftRightJam64.c
    third_party/berkeley-softfloat-3/source/s_shiftRightJamM.c
    third_party/berkeley-softfloat-3/source/s_shiftRightM.c
    third_party/berkeley-softfloat-3/source/s_countLeadingZeros8.c
    third_party/berkeley-softfloat-3/source/s_countLeadingZeros16.c
    third_party/berkeley-softfloat-3/source/s_countLeadingZeros32.c
    third_party/berkeley-softfloat-3/source/s_countLeadingZeros64.c
    third_party/berkeley-softfloat-3/source/s_addM.c
    third_party/berkeley-softfloat-3/source/s_addCarryM.c
    third_party/berkeley-softfloat-3/source/s_addComplCarryM.c
    third_party/berkeley-softfloat-3/source/s_negXM.c
    third_party/berkeley-softfloat-3/source/s_sub1XM.c
    third_party/berkeley-softfloat-3/source/s_subM.c
    third_party/berkeley-softfloat-3/source/s_approxRecip_1Ks.c
    third_party/berkeley-softfloat-3/source/s_approxRecip32_1.c
    third_party/berkeley-softfloat-3/source/s_approxRecipSqrt_1Ks.c
    third_party/berkeley-softfloat-3/source/s_approxRecipSqrt32_1.c
    third_party/berkeley-softfloat-3/source/s_remStepMBy32.c

    # berkeley-softfloat-3 specialize
    third_party/berkeley-softfloat-3/source/RISCV/s_commonNaNToF64UI.c
    third_party/berkeley-softfloat-3/source/RISCV/s_f32UIToCommonNaN.c
    third_party/berkeley-softfloat-3/source/RISCV/s_propagateNaNF64UI.c
    third_party/berkeley-softfloat-3/source/RISCV/specialize.h
    third_party/berkeley-softfloat-3/source/RISCV/s_commonNaNToF32UI.c
    third_party/berkeley-softfloat-3/source/RISCV/s_f64UIToCommonNaN.c
    third_party/berkeley-softfloat-3/source/RISCV/s_propagateNaNF32UI.c
    third_party/berkeley-softfloat-3/source/RISCV/softfloat_raiseFlags.c

    # berkeley-softfloat-3 others
    third_party/berkeley-softfloat-3/source/s_roundToUI32.c
    third_party/berkeley-softfloat-3/source/s_roundMToUI64.c
    third_party/berkeley-softfloat-3/source/s_roundToI32.c
    third_party/berkeley-softfloat-3/source/s_roundMToI64.c
    third_party/berkeley-softfloat-3/source/s_normSubnormalF32Sig.c
    third_party/berkeley-softfloat-3/source/s_roundPackToF32.c
    third_party/berkeley-softfloat-3/source/s_normRoundPackToF32.c
    third_party/berkeley-softfloat-3/source/s_addMagsF32.c
    third_party/berkeley-softfloat-3/source/s_subMagsF32.c
    third_party/berkeley-softfloat-3/source/s_mulAddF32.c
    third_party/berkeley-softfloat-3/source/s_normSubnormalF64Sig.c
    third_party/berkeley-softfloat-3/source/s_roundPackToF64.c
    third_party/berkeley-softfloat-3/source/s_normRoundPackToF64.c
    third_party/berkeley-softfloat-3/source/s_addMagsF64.c
    third_party/berkeley-softfloat-3/source/s_subMagsF64.c
    third_party/berkeley-softfloat-3/source/s_mulAddF64.c
    third_party/berkeley-softfloat-3/source/s_mul64To128M.c
    third_party/berkeley-softfloat-3/source/softfloat_state.c
    third_party/berkeley-softfloat-3/source/ui32_to_f32.c
    third_party/berkeley-softfloat-3/source/ui32_to_f64.c
    third_party/berkeley-softfloat-3/source/ui64_to_f32.c
    third_party/berkeley-softfloat-3/source/ui64_to_f64.c
    third_party/berkeley-softfloat-3/source/i32_to_f32.c
    third_party/berkeley-softfloat-3/source/i32_to_f64.c
    third_party/berkeley-softfloat-3/source/i64_to_f32.c
    third_party/berkeley-softfloat-3/source/i64_to_f64.c
    third_party/berkeley-softfloat-3/source/f32_to_ui32.c
    third_party/berkeley-softfloat-3/source/f32_to_ui64.c
    third_party/berkeley-softfloat-3/source/f32_to_i32.c
    third_party/berkeley-softfloat-3/source/f32_to_i64.c
    third_party/berkeley-softfloat-3/source/f32_to_ui32_r_minMag.c
    third_party/berkeley-softfloat-3/source/f32_to_ui64_r_minMag.c
    third_party/berkeley-softfloat-3/source/f32_to_i32_r_minMag.c
    third_party/berkeley-softfloat-3/source/f32_to_i64_r_minMag.c
    third_party/berkeley-softfloat-3/source/f32_to_f64.c
    third_party/berkeley-softfloat-3/source/f32_roundToInt.c
    third_party/berkeley-softfloat-3/source/f32_add.c
    third_party/berkeley-softfloat-3/source/f32_sub.c
    third_party/berkeley-softfloat-3/source/f32_mul.c
    third_party/berkeley-softfloat-3/source/f32_mulAdd.c
    third_party/berkeley-softfloat-3/source/f32_div.c
    third_party/berkeley-softfloat-3/source/f32_rem.c
    third_party/berkeley-softfloat-3/source/f32_sqrt.c
    third_party/berkeley-softfloat-3/source/f32_eq.c
    third_party/berkeley-softfloat-3/source/f32_le.c
    third_party/berkeley-softfloat-3/source/f32_lt.c
    third_party/berkeley-softfloat-3/source/f32_eq_signaling.c
    third_party/berkeley-softfloat-3/source/f32_le_quiet.c
    third_party/berkeley-softfloat-3/source/f32_lt_quiet.c
    third_party/berkeley-softfloat-3/source/f32_isSignalingNaN.c
    third_party/berkeley-softfloat-3/source/f64_to_ui32.c
    third_party/berkeley-softfloat-3/source/f64_to_ui64.c
    third_party/berkeley-softfloat-3/source/f64_to_i32.c
    third_party/berkeley-softfloat-3/source/f64_to_i64.c
    third_party/berkeley-softfloat-3/source/f64_to_ui32_r_minMag.c
    third_party/berkeley-softfloat-3/source/f64_to_ui64_r_minMag.c
    third_party/berkeley-softfloat-3/source/f64_to_i32_r_minMag.c
    third_party/berkeley-softfloat-3/source/f64_to_i64_r_minMag.c
    third_party/berkeley-softfloat-3/source/f64_to_f32.c
    third_party/berkeley-softfloat-3/source/f64_roundToInt.c
    third_party/berkeley-softfloat-3/source/f64_add.c
    third_party/berkeley-softfloat-3/source/f64_sub.c
    third_party/berkeley-softfloat-3/source/f64_mul.c
    third_party/berkeley-softfloat-3/source/f64_mulAdd.c
    third_party/berkeley-softfloat-3/source/f64_div.c
    third_party/berkeley-softfloat-3/source/f64_rem.c
    third_party/berkeley-softfloat-3/source/f64_sqrt.c
    third_party/berkeley-softfloat-3/source/f64_eq.c
    third_party/berkeley-softfloat-3/source/f64_le.c
    third_party/berkeley-softfloat-3/source/f64_lt.c
    third_party/berkeley-softfloat-3/source/f64_eq_signaling.c
    third_party/berkeley-softfloat-3/source/f64_le_quiet.c
    third_party/berkeley-softfloat-3/source/f64_lt_quiet.c
    third_party/berkeley-softfloat-3/source/f64_isSignalingNaN.c
)

if (MSVC)
    target_compile_options(librafi_fp
        PUBLIC /wd"4101"
        PUBLIC /wd"4102"
        PUBLIC /wd"4146"
        PUBLIC /wd"4206"
        PUBLIC /wd"4244"
        PUBLIC /wd"4389"
        PUBLIC /wd"4701"
        PUBLIC /wd"4703"
        )
endif()

add_library(librafi_trace
    include/rafi/trace.h
    include/rafi/trace/FileTraceReader.h
    include/rafi/trace/FileTraceWriter.h
    include/rafi/trace/ITraceReader.h
    include/rafi/trace/ITraceWriter.h
    include/rafi/trace/MemoryTraceReader.h
    include/rafi/trace/MemoryTraceWriter.h
    include/rafi/trace/CycleBuilder.h
    include/rafi/trace/CycleConfig.h
    include/rafi/trace/CycleReader.h
    include/rafi/trace/CycleTypes.h
    src/librafi_trace/FileTraceReader.cpp
    src/librafi_trace/FileTraceReaderImpl.cpp
    src/librafi_trace/FileTraceReaderImpl.h
    src/librafi_trace/FileTraceWriter.cpp
    src/librafi_trace/FileTraceWriterImpl.cpp
    src/librafi_trace/FileTraceWriterImpl.h
    src/librafi_trace/MemoryTraceReader.cpp
    src/librafi_trace/MemoryTraceReaderImpl.cpp
    src/librafi_trace/MemoryTraceReaderImpl.h
    src/librafi_trace/MemoryTraceWriter.cpp
    src/librafi_trace/MemoryTraceWriterImpl.cpp
    src/librafi_trace/MemoryTraceWriterImpl.h
    src/librafi_trace/CycleBuilder.cpp
    src/librafi_trace/CycleBuilderImpl.cpp
    src/librafi_trace/CycleBuilderImpl.h
    src/librafi_trace/CycleReader.cpp
    src/librafi_trace/CycleReaderImpl.cpp
    src/librafi_trace/CycleReaderImpl.h
)

add_executable(rafi-check-io
    src/rafi-check-io/Main.cpp
)

add_executable(rafi-diff
    src/rafi-diff/Main.cpp
    src/rafi-diff/CycleComparator.cpp
    src/rafi-diff/CycleComparator.h
)

add_executable(rafi-dump-pc
    src/rafi-dump-pc/Main.cpp
)

add_executable(rafi-dump
    src/rafi-dump/Main.cpp
)

add_executable(rafi-emu
    include/rafi/emu.h
    include/rafi/emu/BasicTypes.h
    include/rafi/emu/Event.h
    include/rafi/emu/Exception.h
    include/rafi/emu/IInterruptSource.h
    include/rafi/emu/Macro.h
    include/rafi/emu/MemoryMap.h
    include/rafi/emu/Util.h
    src/rafi-emu/bus/Bus.cpp
    src/rafi-emu/bus/Bus.h
    src/rafi-emu/cpu/Csr.cpp
    src/rafi-emu/cpu/Csr.h
    src/rafi-emu/cpu/CsrTypes.h
    src/rafi-emu/cpu/Executor.cpp
    src/rafi-emu/cpu/Executor.h
    src/rafi-emu/cpu/FpRegFile.cpp
    src/rafi-emu/cpu/FpRegFile.h
    src/rafi-emu/cpu/InterruptController.cpp
    src/rafi-emu/cpu/InterruptController.h
    src/rafi-emu/cpu/IntRegFile.cpp
    src/rafi-emu/cpu/IntRegFile.h
    src/rafi-emu/cpu/MemoryAccessUnit.cpp
    src/rafi-emu/cpu/MemoryAccessUnit.h
    src/rafi-emu/cpu/Processor.cpp
    src/rafi-emu/cpu/Processor.h
    src/rafi-emu/cpu/Trap.cpp
    src/rafi-emu/cpu/Trap.h
    src/rafi-emu/cpu/TrapProcessor.cpp
    src/rafi-emu/cpu/TrapProcessor.h
    src/rafi-emu/io/IIo.h
    src/rafi-emu/io/IoInterruptSource.cpp
    src/rafi-emu/io/IoInterruptSource.h
    src/rafi-emu/io/Uart.cpp
    src/rafi-emu/io/Uart.h
    src/rafi-emu/io/UartTypes.h
    src/rafi-emu/io/Timer.cpp
    src/rafi-emu/io/Timer.h
    src/rafi-emu/mem/IMemory.h
    src/rafi-emu/mem/Ram.cpp
    src/rafi-emu/mem/Ram.h
    src/rafi-emu/mem/Rom.cpp
    src/rafi-emu/mem/Rom.h
    src/rafi-emu/CommandLineOption.cpp
    src/rafi-emu/CommandLineOption.h
    src/rafi-emu/Main.cpp
    src/rafi-emu/Profiler.cpp
    src/rafi-emu/Profiler.h
    src/rafi-emu/System.cpp
    src/rafi-emu/System.h
    src/rafi-emu/TraceDumper.cpp
    src/rafi-emu/TraceDumper.h
)

add_executable(test-op
    test/test-op/test_OpGetString.cpp
)

add_executable(test-trace
    test/test-trace/test_TraceCycle.cpp
    test/test-trace/test_TraceReader.cpp
    test/test-trace/test_TraceUtil.cpp
    test/test-trace/test_TraceUtil.h
    test/test-trace/test_TraceWriter.cpp
)

include_directories(librafi_fp include ${Softfloat_INCLUDE_DIRS})
include_directories(rafi-check-io include)
include_directories(rafi-diff include)
include_directories(rafi-dump include)
include_directories(rafi-dump-pc include)
include_directories(rafi-emu include src/rafi-emu/include)
include_directories(test-op include)
include_directories(test-trace include)

target_link_libraries(rafi-check-io librafi_common librafi_trace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(rafi-diff librafi_common librafi_trace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(rafi-dump librafi_common librafi_trace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(rafi-dump-pc librafi_common librafi_trace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(rafi-emu librafi_common librafi_fp librafi_trace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(test-op librafi_common librafi_trace ${GoogleTest_LIBRARIES})
target_link_libraries(test-trace librafi_common librafi_trace ${GoogleTest_LIBRARIES})
