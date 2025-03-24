cmake_minimum_required(VERSION 3.20)

idf_component_register()

tiny_library_named(tinysettings)
tiny_include_directories(${TINYSETTINGS_DIR}/include)
add_subdirectory(${TINYSETTINGS_DIR}/src)
target_link_libraries(${COMPONENT_LIB} INTERFACE tinysettings)

message(STATUS "Building tinysettings *******************${COMPONENT_LIB}")
