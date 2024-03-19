#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "Makerdiary nRF52840 Connect Kit"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#if QSPI_FLASH_FILESYSTEM
#define MICROPY_QSPI_DATA0                NRF_GPIO_PIN_MAP(0, 20)
#define MICROPY_QSPI_DATA1                NRF_GPIO_PIN_MAP(0, 21)
#define MICROPY_QSPI_DATA2                NRF_GPIO_PIN_MAP(0, 22)
#define MICROPY_QSPI_DATA3                NRF_GPIO_PIN_MAP(0, 23)
#define MICROPY_QSPI_SCK                  NRF_GPIO_PIN_MAP(0, 19)
#define MICROPY_QSPI_CS                   NRF_GPIO_PIN_MAP(0, 17)
#endif

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_24)
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_25)

#define DEFAULT_SPI_BUS_SCK         (&pin_P0_14)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P0_15)
#define DEFAULT_SPI_BUS_MISO        (&pin_P0_16)

#define DEFAULT_UART_BUS_RX         (&pin_P0_12)
#define DEFAULT_UART_BUS_TX         (&pin_P0_13)

#define CIRCUITPY_RGB_STATUS_INVERTED_PWM
#define CIRCUITPY_RGB_STATUS_R      (&pin_P1_10)
#define CIRCUITPY_RGB_STATUS_G      (&pin_P1_11)
#define CIRCUITPY_RGB_STATUS_B      (&pin_P1_12)
