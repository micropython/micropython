include(boards/ESP32_GENERIC/mpconfigboard.cmake)

set(SDKCONFIG_DEFAULTS
    ${SDKCONFIG_DEFAULTS}
    boards/sdkconfig.240mhz
    boards/SIL_WESP32/sdkconfig.board
)
