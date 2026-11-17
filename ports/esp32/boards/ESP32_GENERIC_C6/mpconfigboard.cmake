include(boards/mpconfigboard_esp32c6_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.csi
)
