set(PICO_BOARD "waveshare_rp2040_zero")

# Override the MicroPython board name
list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_BOARD_NAME="Waveshare RP2040-Zero"
)
