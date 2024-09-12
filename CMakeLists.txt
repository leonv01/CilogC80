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

# Gather header files (not directly used but useful)
file(GLOB_RECURSE APP_HEADERS
        src/cpu/*.h
        src/memory/*.h
        src/utils/*.h
)

# Gather test source files
file(GLOB_RECURSE TEST_CPU_SOURCES
        tests/cpu/*.c
)

file(GLOB_RECURSE TEST_REGISTER_SOURCES
        tests/register/*.c
)

# Gather Unity source files
file(GLOB_RECURSE UNITY_SOURCE
        extern/unity/src/*.c
)

# Create the main executable
add_executable(CilogC80
        ${APP_SOURCES}
)

# Create test executables
add_executable(test_cpu
        ${TEST_CPU_SOURCES}
        ${UNITY_SOURCE}
        src/cpu/zilogZ80.c
        src/cpu/register.c
)

add_executable(test_register
        ${TEST_REGISTER_SOURCES}
        ${UNITY_SOURCE}
        src/cpu/register.c
)

# Enable testing and add tests
enable_testing()
add_test(NAME test_cpu COMMAND test_cpu)
add_test(NAME test_register COMMAND test_register)
