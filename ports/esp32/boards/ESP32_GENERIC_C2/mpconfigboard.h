// This configuration is for a generic ESP32C2 board with 4MiB (or more) of flash.

#ifndef MICROPY_HW_BOARD_NAME
#define MICROPY_HW_BOARD_NAME               "ESP32C2 module"
#endif

#ifndef MICROPY_HW_MCU_NAME
#define MICROPY_HW_MCU_NAME                 "ESP32C2"
#endif

#define MICROPY_HW_ENABLE_SDCARD            (0)
#define MICROPY_PY_MACHINE_I2S              (0)

#define MICROPY_BOARD_STARTUP               GENERIC_C2_board_startup
void GENERIC_C2_board_startup(void);
