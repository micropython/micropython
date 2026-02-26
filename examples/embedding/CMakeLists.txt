cmake_minimum_required(VERSION 3.13)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_CXX_COMPILER "clang++")
endif()

# Set build type to reduce firmware size
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE MinSizeRel)
endif()

project(embed)

set(MICROPY_EMBED_CONFIG_DIR ${CMAKE_CURRENT_LIST_DIR})

get_filename_component(MICROPY_EMBED_DIR "${CMAKE_CURRENT_LIST_DIR}/../../ports/embed" ABSOLUTE)

find_package(micropython-embed CONFIG REQUIRED PATHS ${MICROPY_EMBED_DIR})

add_executable(embed-test ${CMAKE_CURRENT_LIST_DIR}/main.c)

target_link_libraries(embed-test PRIVATE micropython)