# 16 MB version of the Pimoroni Pico LiPo board with Typing.py

set(PICO_BOARD "pimoroni_picolipo_16mb")


# Override the MicroPython board name
list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_BOARD_NAME="Pimoroni Pico LiPo 16MB and Typing.py"

    # make sure the C version is not also included to avoid conflicts
    MICROPY_PY_TYPING=0
    MICROPY_PY_TYPING_EXTRA_MODULES=0
)

set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest_TYPING.py)
