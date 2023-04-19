# CMakeLists.txt

# Copyright (c) Mateusz Jandura. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Note: All tests, I've made was built in Visual Studio 2022 which requires at least CMake 3.21.
cmake_minimum_required(VERSION 3.21)
project(safe_list_test
    VERSION 1.0.0
    DESCRIPTION "The safe_list<T> test."
    LANGUAGES CXX
)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

if(NOT (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL Release))
    message(FATAL_ERROR "Requires debug/release build type.")
endif()

# Note: By default, CMAKE_GENERATOR_PLATFORM is equal x64 for 64-bit platforms and Win32 for 32-bit platforms.
#       We need convert it to x64/x86 to link GoogleTest's libraries properly.
if(CMAKE_GENERATOR_PLATFORM STREQUAL x64)
    set(BUILD_PLATFORM "x64")
elseif(CMAKE_GENERATOR_PLATFORM STREQUAL Win32)
    set(BUILD_PLATFORM "x86")
else()
    message(FATAL_ERROR "Requires x64 or x86 platform.")
endif()

add_executable(safe_list_test
    src/inc/safe_list.hpp
    src/test/main.cpp
)

target_compile_features(safe_list_test PRIVATE cxx_std_17)
target_include_directories(safe_list_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/inc)
target_include_directories(safe_list_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/test/thirdparty/GoogleTest/inc)
target_link_libraries(safe_list_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test/thirdparty/GoogleTest/bin/${BUILD_PLATFORM}/${CMAKE_BUILD_TYPE}/gtest.lib)