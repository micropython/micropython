#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "BLE-SS dev board Multi Sensor"
#define MICROPY_HW_MCU_NAME         "nRF52840"
#define MICROPY_HW_LED_STATUS       (&pin_P0_07)

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_26)
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_24)

#define DEFAULT_SPI_BUS_SCK         (&pin_P0_13) /* n.c */
#define DEFAULT_SPI_BUS_MOSI        (&pin_P0_14) /* n.c */
#define DEFAULT_SPI_BUS_MISO        (&pin_P0_15) /* n.c */

#define DEFAULT_UART_BUS_RX         (&pin_P0_02) /* TP7 */
#define DEFAULT_UART_BUS_TX         (&pin_P0_03) /* TP6 */

/* Note: Flash chip is not provided. */
