# cmake file for Raspberry Pi Pico W

set(PICO_BOARD "pico_w")

set(MICROPY_HW_FLASH_SIZE_BYTES "2 * 1024 * 1024")
set(MICROPY_HW_APP_SIZE_BYTES       "1200 * 1024")

set(MICROPY_PY_LWIP ON)
set(MICROPY_PY_NETWORK_CYW43 ON)

# Bluetooth
set(MICROPY_PY_BLUETOOTH ON)
set(MICROPY_BLUETOOTH_BTSTACK ON)
set(MICROPY_PY_BLUETOOTH_CYW43 ON)

# Board specific version of the frozen manifest
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
