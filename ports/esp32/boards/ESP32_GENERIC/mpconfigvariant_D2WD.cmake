set(SDKCONFIG_DEFAULTS
    ${SDKCONFIG_DEFAULTS}
    boards/sdkconfig.flash_2MiB
    boards/ESP32_GENERIC/sdkconfig.d2wd
)

list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_MCU_NAME="ESP32-D2WD"
    # Disable some options to reduce firmware size.
    MICROPY_OPT_COMPUTED_GOTO=0
    MICROPY_PY_NETWORK_LAN=0
)
