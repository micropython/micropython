include(boards/mpconfigboard_esp32s2_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.flash_qio_80m
    boards/UM_FEATHERS2/sdkconfig.board
)

set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
