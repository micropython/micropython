set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.ble
)

if(MICROPY_BOARD_VARIANT STREQUAL "d2wd")
    set(SDKCONFIG_DEFAULTS
        ${SDKCONFIG_DEFAULTS}
        boards/GENERIC/sdkconfig.d2wd
    )

    list(APPEND MICROPY_DEF_BOARD
        MICROPY_HW_MCU_NAME="ESP32-D2WD"
    )
endif()

if(MICROPY_BOARD_VARIANT STREQUAL "ota")
    set(SDKCONFIG_DEFAULTS
        ${SDKCONFIG_DEFAULTS}
        boards/GENERIC/sdkconfig.ota
    )

    list(APPEND MICROPY_DEF_BOARD
        MICROPY_HW_BOARD_NAME="Generic ESP32 module with OTA"
    )
endif()

if(MICROPY_BOARD_VARIANT STREQUAL "spiram")
    set(SDKCONFIG_DEFAULTS
        ${SDKCONFIG_DEFAULTS}
        boards/sdkconfig.spiram
    )

    list(APPEND MICROPY_DEF_BOARD
        MICROPY_HW_BOARD_NAME="Generic ESP32 module with SPIRAM"
    )
endif()

if(MICROPY_BOARD_VARIANT STREQUAL "unicore")
    set(SDKCONFIG_DEFAULTS
        ${SDKCONFIG_DEFAULTS}
        boards/GENERIC/sdkconfig.unicore
    )

    list(APPEND MICROPY_DEF_BOARD
        MICROPY_HW_MCU_NAME="ESP32-UNICORE"
    )
endif()
