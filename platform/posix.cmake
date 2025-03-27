cmake_minimum_required(VERSION 3.20)

tiny_library_named(tinysettings)

# Include tinyplatform
add_subdirectory(${PROJECT_DIR}/../tinyplatform
                 ${PROJECT_DIR}/build/tiny)

tiny_include_directories(${PROJECT_DIR}/include)
tiny_library_link_libraries(tiny)
add_subdirectory(${PROJECT_DIR}/src)
