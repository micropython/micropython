include(boards/mpconfigboard_esp32s3_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS boards/sdkconfig.flash_qio_80m)

# This board has no PSRAM
list(REMOVE_ITEM SDKCONFIG_DEFAULTS boards/sdkconfig.spiram_quad)
