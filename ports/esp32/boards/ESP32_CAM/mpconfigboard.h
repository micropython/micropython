// Both of these can be set by mpconfigboard.cmake if a BOARD_VARIANT is
// specified.

#ifndef MICROPY_HW_BOARD_NAME
#define MICROPY_HW_BOARD_NAME "ESP32S CAM module with PSRAM and OV2640"
#endif

#define MICROPY_PY_BLUETOOTH                (0)
#define MODULE_CAMERA_ENABLED               (1)

#ifndef MICROPY_HW_MCU_NAME
#define MICROPY_HW_MCU_NAME "ESP32"
#endif
