set(IDF_TARGET esp32s3)

set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.ble
    boards/ESP32_GENERIC_S3/sdkconfig.board
)

if(NOT BOARD_NO_SPIRAM_SX)
    # Some board configs don't need the spiram_sx config included
    set(SDKCONFIG_DEFAULTS
        ${SDKCONFIG_DEFAULTS}
        boards/sdkconfig.spiram_sx)
endif()
