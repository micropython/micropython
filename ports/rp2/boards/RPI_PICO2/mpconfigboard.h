// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Raspberry Pi Pico2"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (PICO_FLASH_SIZE_BYTES - 1024 * 1024)

#if MICROPY_PY_NETWORK_CYW43
#include "enable_cyw43.h"

// Enable the ability to pass cyw43 pins into WiFi, Bluetooth and Pin constructors
#define CYW43_PIN_WL_DYNAMIC 1
#define CYW43_PIO_CLOCK_DIV_DYNAMIC 1

// Set the default pins to gpios 2-5
#define CYW43_DEFAULT_PIN_WL_REG_ON 2
#define CYW43_DEFAULT_PIN_WL_CS 3
#define CYW43_DEFAULT_PIN_WL_DATA_OUT 4
#define CYW43_DEFAULT_PIN_WL_DATA_IN 4
#define CYW43_DEFAULT_PIN_WL_HOST_WAKE 4
#define CYW43_DEFAULT_PIN_WL_CLOCK 5

// Default pio clock
#define CYW43_PIO_CLOCK_DIV_INT 3
#endif
