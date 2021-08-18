cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

if (WIN32)
    message(STATUS "Platform is Win32")
    set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "Vcpkg target triplet" FORCE)
elseif (UNIX)
    message(STATUS "Platform is Unix")
    set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "Vcpkg target triplet" FORCE)
endif ()

project(Techluster CXX)

set(CMAKE_CXX_STANDARD 20)
message(STATUS "C++ standard set to 20")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message(STATUS "Compiler is Clang")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "Compiler is GCC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-volatile")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    message(STATUS "Compiler is Intel C++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "Compiler is Visual Studio C++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /MP")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

find_package(Drogon CONFIG REQUIRED)

add_subdirectory(Connect)