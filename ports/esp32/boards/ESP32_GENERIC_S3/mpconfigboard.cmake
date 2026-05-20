set(IDF_TARGET esp32s3)

set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.ble
    boards/sdkconfig.spiram_sx
    boards/ESP32_GENERIC_S3/sdkconfig.board
)

if(MICROPY_PY_ESP32_DS)
    list(APPEND MICROPY_DEF_BOARD
        MICROPY_PY_ESP32_DS=1
    )
endif()
