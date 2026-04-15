# Compute the MicroPython root relative to this component
set(MP_ROOT ${PROJECT_DIR}/../..)

idf_component_register(
  SRCS "modconsts.c"
    PRIV_INCLUDE_DIRS
        ${MP_ROOT}             # micropython/
        ${PROJECT_DIR}         # micropython/ports/esp32
        ${MICROPY_BOARD_DIR}   # board headers (mpconfigboard.h)
        ${CMAKE_CURRENT_LIST_DIR}/..
        ${BUILD_DIR}
    REQUIRES
        micropython
        driver
        freertos)
