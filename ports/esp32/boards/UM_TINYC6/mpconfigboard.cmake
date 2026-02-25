include(boards/mpconfigboard_esp32c6_common.cmake)

list(APPEND
    SDKCONFIG_DEFAULTS
    boards/UM_TINYC6/sdkconfig.board)
