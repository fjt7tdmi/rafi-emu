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

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GoogleTest_INCLUDE_DIRS})

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${GoogleTest_LIBRARY_DIRS})

add_library(librvtrace
    include/rvtrace/common.h
    include/rvtrace/common/Decoder.h
    include/rvtrace/common/Exception.h
    include/rvtrace/common/Op.h
    include/rvtrace/common/RvTypes.h
    include/rvtrace/common/TraceCycleTypes.h
    include/rvtrace/reader.h
    include/rvtrace/reader/FileTraceReader.h
    include/rvtrace/reader/ITraceReader.h
    include/rvtrace/reader/MemoryTraceReader.h
    include/rvtrace/reader/TraceCycleReader.h
    include/rvtrace/writer.h
    include/rvtrace/writer/FileTraceWriter.h
    include/rvtrace/writer/ITraceWriter.h
    include/rvtrace/writer/MemoryTraceWriter.h
    include/rvtrace/writer/TraceCycleBuilder.h
    src/librvtrace/common/Decoder.cpp
    src/librvtrace/common/DecoderImpl.cpp
    src/librvtrace/common/DecoderImpl.h
    src/librvtrace/common/Op.cpp
    src/librvtrace/common/RvTypes.cpp
    src/librvtrace/reader/FileTraceReader.cpp
    src/librvtrace/reader/FileTraceReaderImpl.cpp
    src/librvtrace/reader/FileTraceReaderImpl.h
    src/librvtrace/reader/MemoryTraceReader.cpp
    src/librvtrace/reader/MemoryTraceReaderImpl.cpp
    src/librvtrace/reader/MemoryTraceReaderImpl.h
    src/librvtrace/reader/TraceCycleReader.cpp
    src/librvtrace/reader/TraceCycleReaderImpl.cpp
    src/librvtrace/reader/TraceCycleReaderImpl.h
    src/librvtrace/writer/FileTraceWriter.cpp
    src/librvtrace/writer/FileTraceWriterImpl.cpp
    src/librvtrace/writer/FileTraceWriterImpl.h
    src/librvtrace/writer/MemoryTraceWriter.cpp
    src/librvtrace/writer/MemoryTraceWriterImpl.cpp
    src/librvtrace/writer/MemoryTraceWriterImpl.h
    src/librvtrace/writer/TraceCycleBuilder.cpp
    src/librvtrace/writer/TraceCycleBuilderImpl.cpp
    src/librvtrace/writer/TraceCycleBuilderImpl.h
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
    src/rafi-emu/bus/Bus.cpp
    src/rafi-emu/bus/Bus.h
    src/rafi-emu/cpu/Csr.cpp
    src/rafi-emu/cpu/Csr.h
    src/rafi-emu/cpu/CsrAccessor.cpp
    src/rafi-emu/cpu/CsrAccessor.h
    src/rafi-emu/cpu/CsrTypes.h
    src/rafi-emu/cpu/Decoder.cpp
    src/rafi-emu/cpu/Decoder.h
    src/rafi-emu/cpu/Executor.cpp
    src/rafi-emu/cpu/Executor.h
    src/rafi-emu/cpu/InterruptController.cpp
    src/rafi-emu/cpu/InterruptController.h
    src/rafi-emu/cpu/IntRegFile.cpp
    src/rafi-emu/cpu/IntRegFile.h
    src/rafi-emu/cpu/MemoryAccessUnit.cpp
    src/rafi-emu/cpu/MemoryAccessUnit.h
    src/rafi-emu/cpu/OpTypes.h
    src/rafi-emu/cpu/Processor.cpp
    src/rafi-emu/cpu/Processor.h
    src/rafi-emu/cpu/Trap.cpp
    src/rafi-emu/cpu/Trap.h
    src/rafi-emu/cpu/TrapProcessor.cpp
    src/rafi-emu/cpu/TrapProcessor.h
    src/rafi-emu/include/rafi/BasicTypes.h
    src/rafi-emu/include/rafi/BitField.h
    src/rafi-emu/include/rafi/Common.h
    src/rafi-emu/include/rafi/Event.h
    src/rafi-emu/include/rafi/Exception.h
    src/rafi-emu/include/rafi/IInterruptSource.h
    src/rafi-emu/include/rafi/Util.h
    src/rafi-emu/io/IIo.h
    src/rafi-emu/io/IoInterruptSource.cpp
    src/rafi-emu/io/IoInterruptSource.h
    src/rafi-emu/log/Profiler.cpp
    src/rafi-emu/log/Profiler.h
    src/rafi-emu/log/TraceDumper.cpp
    src/rafi-emu/log/TraceDumper.h
    src/rafi-emu/mem/Memory.cpp
    src/rafi-emu/mem/Memory.h
    src/rafi-emu/uart/Uart.cpp
    src/rafi-emu/uart/Uart.h
    src/rafi-emu/uart/UartTypes.h
    src/rafi-emu/timer/Timer.cpp
    src/rafi-emu/timer/Timer.h
    src/rafi-emu/timer/TimerTypes.h
    src/rafi-emu/Main.cpp
    src/rafi-emu/MemoryMap.h
    src/rafi-emu/System.cpp
    src/rafi-emu/System.h
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

include_directories(rafi-check-io include)
include_directories(rafi-diff include)
include_directories(rafi-dump include)
include_directories(rafi-dump-pc include)
include_directories(rafi-emu include src/rafi-emu/include)
include_directories(test-op include)
include_directories(test-trace include)

target_link_libraries(rafi-check-io librvtrace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(rafi-diff librvtrace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(rafi-dump librvtrace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(rafi-dump-pc librvtrace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(rafi-emu librvtrace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(test-op librvtrace ${GoogleTest_LIBRARIES})
target_link_libraries(test-trace librvtrace ${GoogleTest_LIBRARIES})
