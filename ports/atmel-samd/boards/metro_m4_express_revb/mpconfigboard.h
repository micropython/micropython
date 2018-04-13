// This is for Rev B which a larger run was done and sent to Adafruit community
// members.

#define MICROPY_HW_BOARD_NAME "Metro M4 Express Rev B (Black)"
#define MICROPY_HW_MCU_NAME "samd51j19"

#define CIRCUITPY_MCU_FAMILY samd51

#define MICROPY_HW_LED_TX   PIN_PA27
#define MICROPY_HW_LED_RX   PIN_PB06

#define MICROPY_HW_NEOPIXEL (&pin_PB17)

#define SPI_FLASH_BAUDRATE  (60000000)

// Rev B: single channel SPI
// Rev C will be QSPI
#define SPI_FLASH_MOSI_PIN          PIN_PB08
#define SPI_FLASH_MISO_PIN          PIN_PB11
#define SPI_FLASH_SCK_PIN           PIN_PB09
#define SPI_FLASH_CS_PIN            PIN_PB10
#define SPI_FLASH_MOSI_PIN_FUNCTION PINMUX_PB08D_SERCOM4_PAD0
#define SPI_FLASH_MISO_PIN_FUNCTION PINMUX_PB11D_SERCOM4_PAD3
#define SPI_FLASH_SCK_PIN_FUNCTION  PINMUX_PB09D_SERCOM4_PAD1
#define SPI_FLASH_SERCOM            SERCOM4
#define SPI_FLASH_SERCOM_INDEX      4
#define SPI_FLASH_MOSI_PAD          0
#define SPI_FLASH_MISO_PAD          3
#define SPI_FLASH_SCK_PAD           1
// <o> Transmit Data Pinout
// <0x0=>PAD[0,1]_DO_SCK
// <0x1=>PAD[2,3]_DO_SCK
// <0x2=>PAD[3,1]_DO_SCK
// <0x3=>PAD[0,3]_DO_SCK
#define SPI_FLASH_DOPO              0
#define SPI_FLASH_DIPO              3   // same as MISO pad

// These are pins not to reset.
// Pin for TX LED
#define MICROPY_PORT_A (PORT_PA27)
// Pins for RX LED, SPI flash and neopixel
#define MICROPY_PORT_B (PORT_PB06 | PORT_PB08 | PORT_PB09 | PORT_PB10 | PORT_PB11 | PORT_PB17)
#define MICROPY_PORT_C (0)
#define MICROPY_PORT_D (0)

#define AUTORESET_DELAY_MS 500

#include "external_flash/external_flash.h"

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code
#define CIRCUITPY_INTERNAL_NVM_SIZE 8192

#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#include "external_flash/devices.h"

#define EXTERNAL_FLASH_DEVICE_COUNT 2
#define EXTERNAL_FLASH_DEVICES S25FL216K, \
                               GD25Q16C

#include "external_flash/external_flash.h"
