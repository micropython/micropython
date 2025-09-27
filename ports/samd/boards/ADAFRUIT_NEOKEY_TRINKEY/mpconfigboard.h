#define MICROPY_HW_BOARD_NAME "NeoKey Trinkey"
#define MICROPY_HW_MCU_NAME   "SAMD21E18A"

#define MICROPY_HW_DFLL_USB_SYNC    (1)

// The NEOKEY board has just two accessible GPIO pins.
// So many classes and modules are useless.
#define MICROPY_PY_MACHINE_SOFTI2C  (0)
#define MICROPY_PY_MACHINE_SOFTSPI  (0)
#define MICROPY_PY_MACHINE_I2C      (0)
#define MICROPY_PY_MACHINE_I2C_TARGET (0)
#define MICROPY_PY_MACHINE_SPI      (0)
#define MICROPY_PY_MACHINE_UART     (0)
#define MICROPY_PY_MACHINE_ADC      (0)
#define MICROPY_PY_MACHINE_DAC      (0)
