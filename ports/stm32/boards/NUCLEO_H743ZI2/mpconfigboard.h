#include "boards/NUCLEO_H743ZI/mpconfigboard.h"

#undef MICROPY_HW_BOARD_NAME
#define MICROPY_HW_BOARD_NAME       "NUCLEO_H743ZI2"

// The board has an external 32kHz crystal attached
#undef MICROPY_HW_RTC_USE_LSE
#define MICROPY_HW_RTC_USE_LSE      (1)

// There is no external HS crystal.
// JP1 STLNK_RST will disable the incoming 8MHz clock
// since it is derived from the STLINK's MCO output
#undef MICROPY_HW_CLK_USE_BYPASS
#define MICROPY_HW_CLK_USE_BYPASS   (1)

#undef MICROPY_HW_LED2
#define MICROPY_HW_LED2             (pin_E1)    // yellow

// only when mboot is used
// Define the user button for entering mboot
#if defined(USE_MBOOT)
#define MBOOT_BOOTPIN_PIN (pin_C13)
#define MBOOT_BOOTPIN_PULL (MP_HAL_PIN_PULL_DOWN)
#define MBOOT_BOOTPIN_ACTIVE (1)
#endif
