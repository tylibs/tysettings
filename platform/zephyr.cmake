cmake_minimum_required(VERSION 3.20)

tiny_library_named(tinysettings)

zephyr_library()
zephyr_library_sources(${TINYCOMMON_DIR}/src/empty_file.c)

tiny_include_directories(${TINYSETTINGS_DIR}/include)
add_subdirectory(${TINYSETTINGS_DIR}/src)

zephyr_link_libraries(tinysettings)
