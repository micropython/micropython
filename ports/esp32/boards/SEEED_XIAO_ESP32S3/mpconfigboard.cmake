include(boards/mpconfigboard_esp32s3_common.cmake)

# XIAO ESP32S3 has 8MB flash and 8MB octal SPIRAM.
list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.flash_qio_80m
    boards/sdkconfig.240mhz
    boards/sdkconfig.spiram_oct
)
