cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(NONE)

target_sources(app PRIVATE src/zephyr_start.c src/zephyr_getchar.c)

add_library(libmicropython STATIC IMPORTED)
set_target_properties(libmicropython PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libmicropython.a)
target_link_libraries(app PUBLIC libmicropython)

zephyr_get_include_directories_for_lang_as_string(C includes)
zephyr_get_system_include_directories_for_lang_as_string(C system_includes)
zephyr_get_compile_definitions_for_lang_as_string(C definitions)
zephyr_get_compile_options_for_lang_as_string(C options)

add_custom_target(
  outputexports
  COMMAND echo CC="${CMAKE_C_COMPILER}"
  COMMAND echo AR="${CMAKE_AR}"
  COMMAND echo Z_CFLAGS=${system_includes} ${includes} ${definitions} ${options}
  VERBATIM
  USES_TERMINAL
)
