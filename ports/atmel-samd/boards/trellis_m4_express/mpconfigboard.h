
#define MICROPY_HW_BOARD_NAME "Adafruit Trellis M4 Express"
#define MICROPY_HW_MCU_NAME "samd51g19"

#define CIRCUITPY_MCU_FAMILY samd51

// This is for Rev D
#define MICROPY_HW_APA102_MOSI   (&pin_PB03)
#define MICROPY_HW_APA102_SCK    (&pin_PB02)

#define CIRCUITPY_BITBANG_APA102

// These are pins not to reset.
// QSPI Data pins
#define MICROPY_PORT_A (PORT_PA08 | PORT_PA09 | PORT_PA10 | PORT_PA11)
// DotStar Pins, QSPI CS, and QSPI SCK
#define MICROPY_PORT_B (PORT_PB02 | PORT_PB03 | PORT_PB10 | PORT_PB11)
#define MICROPY_PORT_C (0)
#define MICROPY_PORT_D (0)

#define AUTORESET_DELAY_MS 500

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code
#define CIRCUITPY_INTERNAL_NVM_SIZE 8192

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#define DEFAULT_I2C_BUS_SCL (&pin_PB08)
#define DEFAULT_I2C_BUS_SDA (&pin_PB09)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
