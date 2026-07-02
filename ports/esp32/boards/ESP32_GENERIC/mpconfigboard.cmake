include(boards/mpconfigboard_esp32_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.csi)
