include(boards/mpconfigboard_esp32_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.spiram_esp32
    boards/sdkconfig.240mhz
    boards/sdkconfig.flash_qio_80m
    boards/SIL_MANT1S/sdkconfig.board
)

set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
