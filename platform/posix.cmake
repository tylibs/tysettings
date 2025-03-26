cmake_minimum_required(VERSION 3.20)

tiny_library_named(tinysettings)

# Include tinyplatform
add_subdirectory(${TINYSETTINGS_DIR}/../tinyplatform
                 ${TINYSETTINGS_DIR}/build/tiny)

tiny_include_directories(${TINYSETTINGS_DIR}/include)
tiny_library_link_libraries(tiny)
add_subdirectory(${TINYSETTINGS_DIR}/src)
