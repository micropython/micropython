include(boards/mpconfigboard_esp32s3_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.240mhz
    boards/sdkconfig.flash_qio_80m
    boards/sdkconfig.lwip_fast
    boards/UM_TINYWATCHS3/sdkconfig.board
)
