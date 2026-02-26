include(boards/ESP32_GENERIC_S3/mpconfigboard.cmake)

if(NOT MICROPY_DIR)
    get_filename_component(MICROPY_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../.. ABSOLUTE)
endif()

set(SDKCONFIG_DEFAULTS
    ${SDKCONFIG_DEFAULTS}
    boards/sdkconfig.240mhz
    boards/sdkconfig.spiram_oct
    ${MICROPY_BOARD_DIR}/sdkconfig.board
)

set(MICROPY_SOURCE_BOARD
    ${MICROPY_BOARD_DIR}/board_init.c
    ${MICROPY_BOARD_DIR}/double_tap.c
    ${MICROPY_DIR}/shared/tinyusb/mp_usbd_cdc.c
)

set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
