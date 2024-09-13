set(PICO_BOARD "pimoroni_picolipo_16mb")

# Override the MicroPython board name
list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_BOARD_NAME="Pimoroni Pico LiPo 16MB"
)