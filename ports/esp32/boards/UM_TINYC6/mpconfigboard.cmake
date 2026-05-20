include(boards/ESP32_GENERIC_C6/mpconfigboard.cmake)

set(SDKCONFIG_DEFAULTS
    ${SDKCONFIG_DEFAULTS}
    boards/UM_TINYC6/sdkconfig.board
)
