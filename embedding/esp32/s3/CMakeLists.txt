cmake_minimum_required(VERSION 3.16)

project(micropython_embed)

set(EMBED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/micropython_embed")
set(STATICLIB "micropython")

include_directories(
    .
    "${EMBED_DIR}"
    "${EMBED_DIR}/port"
)

add_compile_options(
    -Wall
    -O3
    -fno-common
    -mlongcalls
)

file(GLOB_RECURSE SRC
    "${EMBED_DIR}/*/*.c"
    "${EMBED_DIR}/*/*/*.c"
)

add_library(${STATICLIB} STATIC ${SRC})

set_target_properties(${STATICLIB} PROPERTIES OUTPUT_NAME "micropython")

install(
    TARGETS micropython
    ARCHIVE DESTINATION /usr/local/lib/esp32s3
)

install(
    FILES
    ./micropython_embed/port/micropython_embed.h
    DESTINATION /usr/local/include/esp32s3
)
