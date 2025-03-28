cmake_minimum_required(VERSION 3.20)

# test if the system is Linux

if(ESP_PLATFORM)
  add_subdirectory(esp)
elseif(ZEPHYR_BASE)
  add_subdirectory(zephyr)
elseif(UNIX)
  add_subdirectory(posix)
else()
  message(FATAL_ERROR "Unsupported platform")

endif()
