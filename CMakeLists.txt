cmake_minimum_required(VERSION 3.26)
project(CilogC80 C)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Include necessary directories
include_directories(src/utils)
include_directories(src/cpu)         
include_directories(src/memory)      
include_directories(extern/unity/src)

file(GLOB_RECURSE APP_SOURCES
        src/main.c
        src/cpu/*.c
        src/memory/*.c
        src/utils/*.c
)


file(GLOB_RECURSE APP_HEADERS
        src/cpu/*.h
        src/memory/*.h
        src/utils/*.h
)

add_executable(CilogC80 ${APP_SOURCES})

add_executable(test_cpu
    tests/test_cpu.c
    extern/unity/src/unity.c
    src/cpu/zilogZ80.c      
    src/cpu/register.c      
)

# Enable testing and add the test
enable_testing()
add_test(NAME test_cpu COMMAND test_cpu)
