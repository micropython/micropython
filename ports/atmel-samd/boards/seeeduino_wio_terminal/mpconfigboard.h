#define MICROPY_HW_BOARD_NAME "Seeeduino Wio Terminal"
#define MICROPY_HW_MCU_NAME "samd51p19"

// Rev E

#define MICROPY_HW_LED_STATUS   (&pin_PA15)

// These are pins not to reset.
// QSPI Data pins
#define MICROPY_PORT_A (PORT_PA08 | PORT_PA09 | PORT_PA10 | PORT_PA11)
// QSPI CS, QSPI SCK and NeoPixel pin
#define MICROPY_PORT_B (PORT_PB03 | PORT_PB10 | PORT_PB11)
#define MICROPY_PORT_C (0)
#define MICROPY_PORT_D (0)

#define EXTERNAL_FLASH_QSPI_DUAL

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL (&pin_PA16)
#define DEFAULT_I2C_BUS_SDA (&pin_PA17)

#define DEFAULT_SPI_BUS_SCK (&pin_PB03)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB02)
#define DEFAULT_SPI_BUS_MISO (&pin_PB00)

#define DEFAULT_UART_BUS_RX (&pin_PB27)
#define DEFAULT_UART_BUS_TX (&pin_PB26)

#define PIN_CTR_5V (&pin_PC14)
#define PIN_CTR_3V3 (&pin_PC15)
#define PIN_USB_HOST_ENABLE (&pin_PA27)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

#define IGNORE_PIN_PA00     1   // XIN32
#define IGNORE_PIN_PA01     1   // XOUT32
#define IGNORE_PIN_PA03     1   // VREFA (VCC3V3)
#define IGNORE_PIN_PA23     1   // NC
#define IGNORE_PIN_PB15     1   // NC
#define IGNORE_PIN_PB22     1   // XIN
#define IGNORE_PIN_PB23     1   // XOUT
#define IGNORE_PIN_PC00     1   // NC (for XOSC32K)
