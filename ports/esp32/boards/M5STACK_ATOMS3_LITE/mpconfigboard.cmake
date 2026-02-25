include(boards/mpconfigboard_esp32s3_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS boards/ESP32_GENERIC_S3/sdkconfig.board)

# This board has no PSRAM
list(REMOVE_ITEM SDKCONFIG_DEFAULTS boards/sdkconfig.spiram_sx)
