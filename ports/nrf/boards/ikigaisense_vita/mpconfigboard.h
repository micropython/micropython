#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "IkigaiSense Vita nRF52840"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define MICROPY_HW_LED_STATUS       (&pin_P0_27)

#define CIRCUITPY_INTERNAL_NVM_SIZE (4096)

#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_08)
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_04)

#define DEFAULT_UART_BUS_RX         (&pin_P0_24)
#define DEFAULT_UART_BUS_TX         (&pin_P0_22)
