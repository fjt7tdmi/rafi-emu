cmake_minimum_required(VERSION 3.8)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

find_package(Boost)

if (NOT ${Boost_FOUND})
    message(FATAL_ERROR "Cannot find boost.")
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(Boost_LIBRARIES "boost_program_options")
    set(FS_LIBRARIES "stdc++fs")
    set(GoogleTest_LIBRARIES "gtest" "gtest_main" "pthread")
else()
    set(GoogleTest_LIBRARIES "gtest" "gtest_main")
endif()

set(GoogleTest_INCLUDE_DIRS "third_party/googletest/googletest/include")
set(GoogleTest_LIBRARY_DIRS "third_party/googletest/build/googletest")

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GoogleTest_INCLUDE_DIRS})

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${GoogleTest_LIBRARY_DIRS})

add_executable(CheckIo
    Sources/Common/BasicTypes.h
    Sources/Common/Exception.h
    Sources/Trace/TraceBinary.h
    Sources/Trace/TraceBinaryReader.cpp
    Sources/Trace/TraceBinaryReader.h
    Sources/Trace/TraceBinaryUtil.cpp
    Sources/Trace/TraceBinaryUtil.h
    Sources/CheckIo.cpp
)

add_executable(CompareTrace
    Sources/Common/BasicTypes.h
    Sources/Common/Exception.h
    Sources/Common/RvTypes.cpp
    Sources/Common/RvTypes.h
    Sources/Trace/TraceBinary.h
    Sources/Trace/TraceBinaryComparator.cpp
    Sources/Trace/TraceBinaryComparator.h
    Sources/Trace/TraceBinaryReader.cpp
    Sources/Trace/TraceBinaryReader.h
    Sources/Trace/TraceBinaryUtil.cpp
    Sources/Trace/TraceBinaryUtil.h
    Sources/Trace/TraceException.h
    Sources/CompareTrace.cpp
)

add_executable(DumpPc
    Sources/Common/BasicTypes.h
    Sources/Common/Exception.h
    Sources/Common/RvTypes.cpp
    Sources/Common/RvTypes.h
    Sources/Trace/TraceBinary.h
    Sources/Trace/TraceBinaryReader.cpp
    Sources/Trace/TraceBinaryReader.h
    Sources/Trace/TraceBinaryUtil.cpp
    Sources/Trace/TraceBinaryUtil.h
    Sources/DumpPc.cpp
)

add_executable(PrintTrace
    Sources/Common/BasicTypes.h
    Sources/Common/Exception.h
    Sources/Common/RvTypes.cpp
    Sources/Common/RvTypes.h
    Sources/Trace/TraceBinary.h
    Sources/Trace/TraceBinaryReader.cpp
    Sources/Trace/TraceBinaryReader.h
    Sources/Trace/TraceBinaryUtil.cpp
    Sources/Trace/TraceBinaryUtil.h
    Sources/PrintTrace.cpp
)

add_executable(RafiEmu
    Sources/Common/BasicTypes.h
    Sources/Common/BitField.h
    Sources/Common/Exception.h
    Sources/Common/RvTypes.cpp
    Sources/Common/RvTypes.h
    Sources/Trace/TraceBinaryWriter.cpp
    Sources/Trace/TraceBinaryWriter.h
    Sources/Profiler/Profiler.cpp
    Sources/Profiler/Profiler.h
    Sources/System/Bus/Bus.cpp
    Sources/System/Bus/Bus.h
    Sources/System/Bus/IBusSlave.h
    Sources/System/Memory/Memory.cpp
    Sources/System/Memory/Memory.h
    Sources/System/Processor/ControlStatusRegister.cpp
    Sources/System/Processor/ControlStatusRegister.h
    Sources/System/Processor/MemoryAccessUnit.cpp
    Sources/System/Processor/MemoryAccessUnit.h
    Sources/System/Processor/Op.cpp
    Sources/System/Processor/Op.h
    Sources/System/Processor/Processor.cpp
    Sources/System/Processor/Processor.h
    Sources/System/Processor/ProcessorException.h
    Sources/System/Processor/RegisterFile.cpp
    Sources/System/Processor/RegisterFile.h
    Sources/System/Uart/Uart.cpp
    Sources/System/Uart/Uart.h
    Sources/System/System.cpp
    Sources/System/System.h
    Sources/RafiEmu.cpp
)

add_executable(SampleTest
    Tests/SampleTest.cpp
)

target_link_libraries(CheckIo ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(CompareTrace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(DumpPc ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(PrintTrace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(RafiEmu ${Boost_LIBRARIES})
target_link_libraries(SampleTest ${GoogleTest_LIBRARIES})
