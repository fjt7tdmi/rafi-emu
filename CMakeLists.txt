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

add_executable(CheckIo
    Sources/Common/BasicTypes.h
    Sources/Common/Exception.h
    Sources/OldTrace/TraceBinary.h
    Sources/OldTrace/TraceBinaryReader.cpp
    Sources/OldTrace/TraceBinaryReader.h
    Sources/OldTrace/TraceBinaryUtil.cpp
    Sources/OldTrace/TraceBinaryUtil.h
    Sources/CheckIo.cpp
)

add_executable(CompareTrace
    Sources/Common/BasicTypes.h
    Sources/Common/Exception.h
    Sources/Common/RvTypes.cpp
    Sources/Common/RvTypes.h
    Sources/OldTrace/TraceBinary.h
    Sources/OldTrace/TraceBinaryComparator.cpp
    Sources/OldTrace/TraceBinaryComparator.h
    Sources/OldTrace/TraceBinaryReader.cpp
    Sources/OldTrace/TraceBinaryReader.h
    Sources/OldTrace/TraceBinaryUtil.cpp
    Sources/OldTrace/TraceBinaryUtil.h
    Sources/Trace/TraceException.h
    Sources/CompareTrace.cpp
)

add_executable(DumpPc
    Sources/Common/BasicTypes.h
    Sources/Common/Exception.h
    Sources/Common/RvTypes.cpp
    Sources/Common/RvTypes.h
    Sources/OldTrace/TraceBinary.h
    Sources/OldTrace/TraceBinaryReader.cpp
    Sources/OldTrace/TraceBinaryReader.h
    Sources/OldTrace/TraceBinaryUtil.cpp
    Sources/OldTrace/TraceBinaryUtil.h
    Sources/DumpPc.cpp
)

add_executable(PrintTrace
    Sources/Common/BasicTypes.h
    Sources/Common/Exception.h
    Sources/Common/RvTypes.cpp
    Sources/Common/RvTypes.h
    Sources/OldTrace/TraceBinary.h
    Sources/OldTrace/TraceBinaryReader.cpp
    Sources/OldTrace/TraceBinaryReader.h
    Sources/OldTrace/TraceBinaryUtil.cpp
    Sources/OldTrace/TraceBinaryUtil.h
    Sources/PrintTrace.cpp
)

add_executable(RafiEmu
    Sources/Common/BasicTypes.h
    Sources/Common/BitField.h
    Sources/Common/Exception.h
    Sources/Common/RvTypes.cpp
    Sources/Common/RvTypes.h
    Sources/OldTrace/TraceBinaryWriter.cpp
    Sources/OldTrace/TraceBinaryWriter.h
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

add_executable(test_Trace
    Sources/Trace/ITraceReader.h
    Sources/Trace/ITraceWriter.h
    Sources/Trace/MemoryTraceReader.cpp
    Sources/Trace/MemoryTraceReader.h
    Sources/Trace/MemoryTraceWriter.cpp
    Sources/Trace/MemoryTraceWriter.h
    Sources/Trace/TraceCycleBuilder.h
    Sources/Trace/TraceCycleBuilder.cpp
    Sources/Trace/TraceCycleCommon.h
    Sources/Trace/TraceCycleException.h
    Sources/Trace/TraceCycleNode.h
    Sources/Trace/TraceCycleReader.cpp
    Sources/Trace/TraceCycleReader.h
    Tests/Trace/test_TraceCycle.cpp
    Tests/Trace/test_TraceReader.cpp
    Tests/Trace/test_TraceUtil.cpp
    Tests/Trace/test_TraceUtil.h
    Tests/Trace/test_TraceWriter.cpp
)

target_link_libraries(CheckIo ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(CompareTrace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(DumpPc ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(PrintTrace ${Boost_LIBRARIES} ${FS_LIBRARIES})
target_link_libraries(RafiEmu ${Boost_LIBRARIES})
target_link_libraries(test_Trace ${GoogleTest_LIBRARIES})
