cmake_minimum_required(VERSION 3.20)

tiny_library_sources(${CMAKE_CURRENT_SOURCE_DIR}/settings.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/settings_file.cpp)

tiny_library_include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(CONFIG_TINYSETTINGS_LOG_LEVEL)
  set(CONFIG_TINYSETTINGS_LOG_LEVEL ${CONFIG_TINYSETTINGS_LOG_LEVEL})
else()
  set(CONFIG_TINYSETTINGS_LOG_LEVEL TY_LOG_LEVEL_CRIT)
endif()

tiny_library_compile_definitions(
  -DTINYSETTINGS_CONFIG_LOG_LEVEL=${CONFIG_TINYSETTINGS_LOG_LEVEL}
  -DTINYSETTINGS_PLATFORM_CONFIG_FILE="tinysettings-posix-config.h")
