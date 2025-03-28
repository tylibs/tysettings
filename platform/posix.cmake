cmake_minimum_required(VERSION 3.20)

ty_library_named(tysettings)
ty_library_link_libraries(tiny)
ty_library_include_directories_public(${PROJECT_DIR}/include)

add_subdirectory(${PROJECT_DIR}/src)
