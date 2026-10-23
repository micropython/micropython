list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.flash_2MiB
    boards/ESP32_GENERIC/sdkconfig.d2wd
)

# The 2MiB D2WD build has no room for the full frozen set alongside BLE under
# IDF v6. The slim manifest keeps asyncio and the hardware libs; everything
# dropped here (umqtt, aioespnow, upysh and the networking bundle) installs
# from micropython-lib via mip when needed.
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest_d2wd.py)

list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_MCU_NAME="ESP32-D2WD"
    # Disable some options to reduce firmware size.
    MICROPY_OPT_COMPUTED_GOTO=0
    MICROPY_PY_NETWORK_LAN=0
)
