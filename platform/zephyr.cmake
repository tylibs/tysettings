cmake_minimum_required(VERSION 3.20)

tiny_library_named(tinysettings)
tiny_library_link_libraries(zephyr_interface)
tiny_library_link_libraries(tiny)

# we will call zephyr kernel functions in scheduler (k_msgq_get, k_msgq_put)
tiny_library_link_libraries(kernel)

tiny_library_include_directories_public(${PROJECT_DIR}/include)
add_subdirectory(${PROJECT_DIR}/src)
zephyr_library()
zephyr_library_sources(${TINYCOMMON_DIR}/src/empty_file.c)
