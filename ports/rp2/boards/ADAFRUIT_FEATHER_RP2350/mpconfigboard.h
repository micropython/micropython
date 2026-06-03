// https://www.adafruit.com/product/6000
// https://learn.adafruit.com/adafruit-feather-rp2350/pinouts

#define MICROPY_HW_BOARD_NAME          "Adafruit Feather RP2350"

#define MICROPY_HW_USB_VID (0x239A)
#define MICROPY_HW_USB_PID (0x814F)

// NeoPixel GPIO21, power not toggleable

// Red user LED GPIO7

// Optional PSRAM on GPIO8
#define MICROPY_HW_PSRAM_CS_PIN (8)
#define MICROPY_HW_ENABLE_PSRAM (1)
