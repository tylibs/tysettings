cmake_minimum_required(VERSION 3.20)

ty_library_named(tysettings)
ty_library_link_libraries(zephyr_interface)
ty_library_link_libraries(tiny)

# we will call zephyr kernel functions in scheduler (k_msgq_get, k_msgq_put)
ty_library_link_libraries(kernel)

ty_library_include_directories_public(${PROJECT_DIR}/include)
add_subdirectory(${PROJECT_DIR}/src)
zephyr_library()
zephyr_library_sources(${TYCOMMON_DIR}/src/empty_file.c)
