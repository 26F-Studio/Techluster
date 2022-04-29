cmake_minimum_required(VERSION 3.18)

include(utils.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

message(STATUS "C++ standard set to ${CMAKE_CXX_STANDARD}")

project(Techluster CXX)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message(STATUS "Compiler is Clang")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "Compiler is GCC")
    add_compile_options(-Wno-volatile -fcoroutines)
    find_library(GCC_ATOMIC NAMES atomic atomic.so.1 libatomic.so.1)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    message(STATUS "Compiler is Intel C++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "Compiler is Visual Studio C++")
    get_WIN32_WINNT(WIN32_WINNT)
    add_compile_options(/MP /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX _WIN32_WINNT=${WIN32_WINNT})
endif ()

find_library(CPPREDIS_LIB cpp_redis)
find_library(TACOPIE_LIB tacopie)
find_package(Drogon CONFIG REQUIRED)
find_package(mailio CONFIG REQUIRED)

add_subdirectory(Shared)
add_subdirectory(Connect)
add_subdirectory(Gaming)
add_subdirectory(Message)
add_subdirectory(Forwarding)
add_subdirectory(User)

# To disable tests, comment out the following lines
enable_testing()
add_subdirectory(Test)