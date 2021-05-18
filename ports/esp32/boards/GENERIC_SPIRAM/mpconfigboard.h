#define MICROPY_HW_BOARD_NAME "ESP32 module (spiram)"
#define MICROPY_HW_MCU_NAME "ESP32"

// Enables/Disables ADC2
// Warning ADC2 conflicts with Wi-Fi
#define MICROPY_PY_MACHINE_ADC_ENABLE_ADC2  (1)
