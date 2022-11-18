#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "BastBLE"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#if QSPI_FLASH_FILESYSTEM
#define MICROPY_QSPI_DATA0                NRF_GPIO_PIN_MAP(0, 30)
#define MICROPY_QSPI_DATA1                NRF_GPIO_PIN_MAP(0, 29)
#define MICROPY_QSPI_DATA2                NRF_GPIO_PIN_MAP(0, 28)
#define MICROPY_QSPI_DATA3                NRF_GPIO_PIN_MAP(0, 2)
#define MICROPY_QSPI_SCK                  NRF_GPIO_PIN_MAP(0, 3)
#define MICROPY_QSPI_CS                   NRF_GPIO_PIN_MAP(0, 26)
#endif

#define DEFAULT_I2C_BUS_SCL         (&pin_P1_10)
#define DEFAULT_I2C_BUS_SDA         (&pin_P1_11)

#define DEFAULT_SPI_BUS_SCK         (&pin_P1_00)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P1_06)
#define DEFAULT_SPI_BUS_MISO        (&pin_P0_15)

#define DEFAULT_UART_BUS_RX         (&pin_P0_09)
#define DEFAULT_UART_BUS_TX         (&pin_P0_10)
