set(IDF_TARGET esp32s3)

set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.usb
    boards/sdkconfig.spiram_sx
    boards/GENERIC_S3_SPIRAM/sdkconfig.board
)
