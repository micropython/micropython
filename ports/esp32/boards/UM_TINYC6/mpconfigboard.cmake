include(boards/mpconfigboard_esp32c6_common.cmake)

list(APPEND
    SDKCONFIG_DEFAULTS
    boards/sdkconfig.flash_qio_80m
    boards/UM_TINYC6/sdkconfig.board)
