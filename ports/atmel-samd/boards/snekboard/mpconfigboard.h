#define MICROPY_HW_BOARD_NAME "keithp.com snekboard"
#define MICROPY_HW_MCU_NAME "samd21g18"

#define MICROPY_HW_LED_STATUS   (&pin_PA02)

#define MICROPY_HW_NEOPIXEL (&pin_PB11)

#define SPI_FLASH_MOSI_PIN          &pin_PB22
#define SPI_FLASH_MISO_PIN          &pin_PB03
#define SPI_FLASH_SCK_PIN           &pin_PB23
#define SPI_FLASH_CS_PIN            &pin_PA27

// These are pins not to reset.
#define MICROPY_PORT_A        (PORT_PB11)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define BOARD_HAS_CRYSTAL 0

#define DEFAULT_I2C_BUS_SCL (&pin_PA08) /* ANALOG 5 */
#define DEFAULT_I2C_BUS_SDA (&pin_PA09) /* ANALOG 6 */

#define DEFAULT_UART_BUS_RX (&pin_PB08) /* ANALOG 1 */
#define DEFAULT_UART_BUS_TX (&pin_PB09) /* ANALOG 2 */

// Other some pins that do not appear in the pinout & are not used internally
// this list is not (yet) exhaustive
#define IGNORE_PIN_PA01     1
#define IGNORE_PIN_PA03     1
#define IGNORE_PIN_PB04     1
#define IGNORE_PIN_PB05     1
#define IGNORE_PIN_PB00     1
#define IGNORE_PIN_PB01     1
#define IGNORE_PIN_PB02     1
#define IGNORE_PIN_PB04     1
#define IGNORE_PIN_PB05     1
#define IGNORE_PIN_PB06     1
#define IGNORE_PIN_PB07     1
#define IGNORE_PIN_PB12     1

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
