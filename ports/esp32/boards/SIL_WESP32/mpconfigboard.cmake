include(boards/mpconfigboard_esp32_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.240mhz
    boards/sdkconfig.flash_qio_80m
    boards/SIL_WESP32/sdkconfig.board
)
