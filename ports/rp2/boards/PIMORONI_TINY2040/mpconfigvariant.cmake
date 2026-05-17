set(PICO_BOARD "pimoroni_tiny2040_2mb")

# Override the MicroPython board name
list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_BOARD_NAME="Pimoroni Tiny 2040 2MB"
)