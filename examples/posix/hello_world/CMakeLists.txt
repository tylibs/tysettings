# SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CONFIG_TINY_LOG_LEVEL TINY_LOG_LEVEL_INFO)

project(hello_world)

set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
# Include tinyplatform
add_subdirectory(${PROJECT_DIR}/../tinyplatform ${PROJECT_DIR}/build/tiny)
# https://cmake.org/pipermail/cmake/2019-June/069547.html
add_subdirectory(${PROJECT_DIR} ${PROJECT_DIR}/build/tinysettings)

add_executable(app)
target_link_libraries(app PRIVATE tinysettings)

# Application Files
add_subdirectory(src)
