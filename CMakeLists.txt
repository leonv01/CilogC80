cmake_minimum_required(VERSION 3.26)
project(CilogC80 C)

# Set the C standard
set(CMAKE_C_STANDARD 99)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
endif()

# Include necessary directories
include_directories(src/utils)
include_directories(src/cpu)
include_directories(src/memory)
include_directories(extern/unity/src)


file(GLOB MAIN_SOURCE
        src/main.c
)

file(GLOB_RECURSE APP_SOURCES
        src/cpu/*.c
        src/memory/*.c
        src/utils/*.c
)

add_executable(CilogC80 ${APP_SOURCES} ${MAIN_SOURCE})

# Gather Unity source files
file(GLOB UNITY_SOURCE
        extern/unity/src/unity.c
)

# Test for CPU
file(GLOB TEST_CPU_SOURCES
        tests/cpu/test_cpu.c
)

add_executable(test_cpu ${TEST_CPU_SOURCES} ${UNITY_SOURCE} ${APP_SOURCES})

# Test for Memory
file(GLOB TEST_MEMORY_SOURCES
        tests/memory/test_mem.c
)

add_executable(test_memory ${TEST_MEMORY_SOURCES} ${UNITY_SOURCE} ${APP_SOURCES})

# Enable testing and add tests
enable_testing()
add_test(NAME test_cpu COMMAND test_cpu)
add_test(NAME test_memory COMMAND test_memory)
