#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "BastBLE"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define CIRCUITPY_AUTORELOAD_DELAY_MS 500

#define DEFAULT_I2C_BUS_SCL         (&pin_P1_10)
#define DEFAULT_I2C_BUS_SDA         (&pin_P1_11)

#define DEFAULT_SPI_BUS_SCK         (&pin_P1_00)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P1_06)
#define DEFAULT_SPI_BUS_MISO        (&pin_P0_15)

#define DEFAULT_UART_BUS_RX         (&pin_P0_09)
#define DEFAULT_UART_BUS_TX         (&pin_P0_10)
