cmake_minimum_required(VERSION 3.20)

# CHECK needed for esp-idf build system to avoid errors if(NOT
# CMAKE_BUILD_EARLY_EXPANSION)
if(NOT CMAKE_BUILD_EARLY_EXPANSION)
  message(STATUS "Building tinylibs for ESP-IDF *******************")
  tiny_library_named(tinysettings)
  # Include tinyplatform
  add_subdirectory(${TINYSETTINGS_DIR}/lib/tinyplatform
                   ${TINYSETTINGS_DIR}/lib/tinyplatform)

  tiny_include_directories(${TINYSETTINGS_DIR}/include)
  add_subdirectory(${TINYSETTINGS_DIR}/src)
endif()

idf_component_register()
