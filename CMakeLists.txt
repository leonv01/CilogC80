cmake_minimum_required(VERSION 3.26)
project(CilogC80 C)

# Set the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
# Set Clang as the compiler
#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)

# Raygui
# ----------------------------------------- #
include_directories(extern/raygui/src)
# ----------------------------------------- #

# Raylib
# ----------------------------------------- #
add_subdirectory(extern/raylib)
set(RAYLIB_STATIC ON)
set(RAYLIB_SOURCE_PATH extern/raygui/src)
# ----------------------------------------- #

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
include_directories(src/emulator)
include_directories(src/emulator/gui)
include_directories(extern/unity/src)

file(GLOB MAIN_SOURCE
        src/main.c
)

file(GLOB_RECURSE APP_SOURCES
        src/cpu/*.c
        src/memory/*.c
        src/utils/*.c
        src/emulator/*.c
        src/emulator/gui/*.c
)

add_executable(CilogC80 ${APP_SOURCES} ${MAIN_SOURCE} )

target_link_libraries(CilogC80 PRIVATE raylib)

target_compile_definitions(CilogC80 PRIVATE RAYLIB_STATIC)
