set(MICROPY_PY_LWIP ON)

list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_NETWORK_USBNET=1
)
# Board specific version of the frozen manifest
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/../RPI_PICO_W/manifest.py)