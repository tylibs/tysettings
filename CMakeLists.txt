cmake_minimum_required(VERSION 3.20.0)

# check for posix build
set(POSIX_SYSTEM_NAMES "AIX;HP-UX;Darwin;Linux;SCO_SV;UnixWare;OS/360")
if(CMAKE_SYSTEM_NAME IN_LIST POSIX_SYSTEM_NAMES)
  set(POSIX_PLATFORM TRUE)
endif()

set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(TINYSETTINGS_DIR ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_MODULE_PATH "${TINYSETTINGS_DIR}/platform"
                      "${TINYSETTINGS_DIR}/lib/tinyplatform/cmake")

# add build extensions for tinylibs
include(extensions)

# build the platform specific compoentents
if(ESP_PLATFORM)
  include(esp)
elseif(ZEPHYR_BASE)
  include(zephyr)
elseif(POSIX_PLATFORM)
  include(posix)
else()
  message(FATAL_ERROR "Unsupported platform")
endif()
