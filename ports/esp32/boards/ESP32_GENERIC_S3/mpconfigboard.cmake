include(boards/mpconfigboard_esp32s3_common.cmake)

list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.flash_qio_80m
    boards/sdkconfig.csi
)

if(MICROPY_PY_ESP32_DS)
    list(APPEND MICROPY_DEF_BOARD
        MICROPY_PY_ESP32_DS=1
    )
endif()
