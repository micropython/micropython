include(boards/ESP32_GENERIC/mpconfigboard.cmake)

set(SDKCONFIG_DEFAULTS
    ${SDKCONFIG_DEFAULTS}
    boards/sdkconfig.spiram
    boards/sdkconfig.240mhz
    boards/SIL_MANT1S/sdkconfig.board
)

set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
