# SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CONFIG_TINY_LOG_LEVEL TINY_LOG_LEVEL_INFO)

project(hello_world)

# include tinysettings module
#
# Note the second, binary_dir parameter requires the added subdirectory to have
# its own, local cmake target(s). If not then this binary_dir is created but
# stays empty. Object files land in the main binary dir instead.
# https://cmake.org/pipermail/cmake/2019-June/069547.html
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../..
                 ${CMAKE_CURRENT_BINARY_DIR}/../../..)

add_executable(app)
target_link_libraries(app PUBLIC tinysettings)

# Application Files
add_subdirectory(src)
