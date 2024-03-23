set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.spiram
)

#    boards/sdkconfig.ble
#    boards/ESP32_CAM/sdkconfig.esp32cam

list(APPEND MICROPY_DEF_BOARD
        MICROPY_HW_MCU_NAME="ESP32"
        # Disable some options to reduce firmware size.
        # MICROPY_OPT_COMPUTED_GOTO=0
        # MICROPY_PY_NETWORK_LAN=0
	# ESP32-CAMERA
	CONFIG_OV2640_SUPPORT=y
        MICROPY_HW_BOARD_NAME="ESP32S CAM module with SPIRAM and OV2640"
	CONFIG_COMPILER_OPTIMIZATION_SIZE=n
	CONFIG_COMPILER_OPTIMIZATION_PERF=y
)


if(MICROPY_BOARD_VARIANT STREQUAL "OTA")
    set(SDKCONFIG_DEFAULTS
        ${SDKCONFIG_DEFAULTS}
        boards/ESP32_GENERIC/sdkconfig.ota
    )

    list(APPEND MICROPY_DEF_BOARD
        MICROPY_HW_BOARD_NAME="Generic ESP32 module with OTA"
    )
endif()



if(MICROPY_BOARD_VARIANT STREQUAL "UNICORE")
    set(SDKCONFIG_DEFAULTS
        ${SDKCONFIG_DEFAULTS}
        boards/ESP32_GENERIC/sdkconfig.unicore
    )

    list(APPEND MICROPY_DEF_BOARD
        MICROPY_HW_MCU_NAME="ESP32-UNICORE"
    )
endif()
