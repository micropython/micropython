#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "ARAMCON Badge 2019"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define MICROPY_HW_LED_STATUS          (&pin_P1_11)

#ifdef QSPI_FLASH_FILESYSTEM
#define MICROPY_QSPI_DATA0                NRF_GPIO_PIN_MAP(0, 22)
#define MICROPY_QSPI_DATA1                NRF_GPIO_PIN_MAP(0, 20)
#define MICROPY_QSPI_DATA2                NRF_GPIO_PIN_MAP(1, 4)
#define MICROPY_QSPI_DATA3                NRF_GPIO_PIN_MAP(1, 6)
#define MICROPY_QSPI_SCK                  NRF_GPIO_PIN_MAP(1, 0)
#define MICROPY_QSPI_CS                   NRF_GPIO_PIN_MAP(1, 2)
#endif

#ifdef SPI_FLASH_FILESYSTEM
#define SPI_FLASH_MOSI_PIN &pin_P0_20
#define SPI_FLASH_MISO_PIN &pin_P0_22
#define SPI_FLASH_SCK_PIN &pin_P1_00
#define SPI_FLASH_CS_PIN &pin_P1_02
#endif

#define BOARD_HAS_32KHZ_XTAL 0

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_28)
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_03)

#define DEFAULT_SPI_BUS_SCK         (&pin_P0_01)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P1_10)
#define DEFAULT_SPI_BUS_MISO        (&pin_P1_09)
