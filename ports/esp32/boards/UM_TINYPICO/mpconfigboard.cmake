include(boards/mpconfigboard_esp32_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.240mhz
    boards/sdkconfig.spiram
    boards/sdkconfig.flash_qio_80m
    boards/UM_TINYPICO/sdkconfig.board
)

set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
