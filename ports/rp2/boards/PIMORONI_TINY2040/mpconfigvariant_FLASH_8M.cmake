set(PICO_BOARD "pimoroni_tiny2040")

# Override the MicroPython board name
list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_BOARD_NAME="Pimoroni Tiny 2040 8MB"
)