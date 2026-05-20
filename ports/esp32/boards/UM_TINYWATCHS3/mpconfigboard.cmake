include(boards/ESP32_GENERIC_S3/mpconfigboard.cmake)

set(SDKCONFIG_DEFAULTS
    ${SDKCONFIG_DEFAULTS}
    boards/sdkconfig.240mhz
    boards/UM_TINYWATCHS3/sdkconfig.board
)
