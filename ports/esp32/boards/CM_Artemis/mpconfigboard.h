#define MICROPY_HW_BOARD_NAME "Artemis watch"
#define MICROPY_HW_MCU_NAME "ESP32S3"

#define MICROPY_PY_MACHINE_DAC              (0)

// Enable UART REPL for modules that have an external USB-UART and don't use native USB. - set to 1 to enable
#define MICROPY_HW_ENABLE_UART_REPL         (0)

#define MICROPY_HW_I2C0_SCL                 (9)
#define MICROPY_HW_I2C0_SDA                 (8)

#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args)     ARTEMIS_ESP32_enter_bootloader()
void ARTEMIS_ESP32_enter_bootloader(void);

