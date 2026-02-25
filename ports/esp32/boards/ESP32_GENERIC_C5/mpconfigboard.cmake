include(boards/mpconfigboard_esp32c5_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS
    boards/ESP32_GENERIC_C5/sdkconfig.board)
