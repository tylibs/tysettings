cmake_minimum_required(VERSION 3.20)

ty_library_sources(${CMAKE_CURRENT_SOURCE_DIR}/settings.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/settings_file.cpp)

ty_library_include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(CONFIG_TYSETTINGS_LOG_LEVEL)
  set(CONFIG_TYSETTINGS_LOG_LEVEL ${CONFIG_TYSETTINGS_LOG_LEVEL})
else()
  set(CONFIG_TYSETTINGS_LOG_LEVEL TY_LOG_LEVEL_CRIT)
endif()

ty_library_compile_definitions(
  -DTYSETTINGS_CONFIG_LOG_LEVEL=${CONFIG_TYSETTINGS_LOG_LEVEL}
  -DTYSETTINGS_PLATFORM_CONFIG_FILE="tysettings-posix-config.h")
