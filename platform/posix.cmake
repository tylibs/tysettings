cmake_minimum_required(VERSION 3.20)

tiny_library_named(tinysettings)
tiny_library_link_libraries(tiny)
tiny_library_include_directories_public(${PROJECT_DIR}/include)

add_subdirectory(${PROJECT_DIR}/src)
