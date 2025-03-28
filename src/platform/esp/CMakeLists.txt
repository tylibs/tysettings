cmake_minimum_required(VERSION 3.20)

ty_library_sources(${CMAKE_CURRENT_SOURCE_DIR}/settings.c)

ty_library_include_directories(${CMAKE_CURRENT_SOURCE_DIR})

ty_library_compile_definitions(
  -DTINYSETTINGS_CONFIG_LOG_LEVEL=${CONFIG_TINYSETTINGS_LOG_LEVEL}
  -DTINYSETTINGS_PLATFORM_CONFIG_FILE="tinysettings-esp-config.h")
