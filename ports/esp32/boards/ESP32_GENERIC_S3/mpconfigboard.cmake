include(boards/mpconfigboard_esp32s3_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS
    boards/ESP32_GENERIC_S3/sdkconfig.board)
