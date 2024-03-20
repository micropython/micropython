#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "SSCI ISP1807 Micro Board"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_23)
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_29)

#define DEFAULT_SPI_BUS_SCK         (&pin_P0_06)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P0_13)
#define DEFAULT_SPI_BUS_MISO        (&pin_P0_08)

#define DEFAULT_UART_BUS_RX         (&pin_P0_19)
#define DEFAULT_UART_BUS_TX         (&pin_P0_30)
