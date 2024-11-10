set(PICO_BOARD "pimoroni_picolipo_16mb")

# Override the MicroPython board name
list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_BOARD_NAME="Pimoroni Pico LiPo 16MB and Typing.c"

    # make sure the C version is included
    MICROPY_PY_TYPING=1
    MICROPY_PY_TYPING_EXTRA_MODULES=1
)
