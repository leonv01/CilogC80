cmake_minimum_required(VERSION 3.26)
project(CilogC80 C)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Include necessary directories
include_directories(src/utils)
include_directories(src/cpu)
include_directories(src/memory)
include_directories(extern/unity/src)

# Gather source files for the main application
file(GLOB_RECURSE APP_SOURCES
        src/cpu/*.c
        src/memory/*.c
        src/utils/*.c
        src/main.c
)

# Create the main executable
add_executable(CilogC80 ${APP_SOURCES})

# Gather Unity source files
file(GLOB UNITY_SOURCE
        extern/unity/src/unity.c
)

# Test for CPU
file(GLOB TEST_CPU_SOURCES
        tests/cpu/test_cpu.c  # This should point to the CPU test file that contains main()
)

add_executable(test_cpu ${TEST_CPU_SOURCES} ${UNITY_SOURCE} src/cpu/cpu.c)

# Test for Memory
file(GLOB TEST_MEMORY_SOURCES
        tests/memory/test_mem.c  # This should point to the Memory test file that contains main()
)

add_executable(test_memory ${TEST_MEMORY_SOURCES} ${UNITY_SOURCE} src/memory/mem.c)

# Enable testing and add tests
enable_testing()
add_test(NAME test_cpu COMMAND test_cpu)
add_test(NAME test_memory COMMAND test_memory)
