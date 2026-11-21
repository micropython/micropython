include(boards/mpconfigboard_esp32p4_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.p4_wifi_common
    boards/SPARKFUN_IOT_REDBOARD_ESP32_P4/sdkconfig.board
)
