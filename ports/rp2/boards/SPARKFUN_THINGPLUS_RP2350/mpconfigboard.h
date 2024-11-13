// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME          "SparkFun Thing Plus RP2350"
#define MICROPY_HW_FLASH_STORAGE_BYTES (15 * 1024 * 1024)

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "ThingPlusRP2350"

#define MICROPY_HW_USB_VID (0x1B4F)
#define MICROPY_HW_USB_PID (0x0038)

#define MICROPY_HW_I2C0_SDA  (6)
#define MICROPY_HW_I2C0_SCL  (7)

#define MICROPY_HW_SPI1_SCK  (2)
#define MICROPY_HW_SPI1_MOSI (3)
#define MICROPY_HW_SPI1_MISO (3)

#define MICROPY_HW_PSRAM_CS_PIN (8)
#define MICROPY_HW_ENABLE_PSRAM (1)

#include "enable_cyw43.h"