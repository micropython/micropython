#include "asf/common2/services/delay/delay.h"
#include "asf/sam0/drivers/port/port.h"

#include "mphalport.h"

#include "shared-bindings/nativeio/DigitalInOut.h"
#include "shared-bindings/neopixel_write/__init__.h"
#include "neopixel_status.h"
#include "samd21_pins.h"

#ifdef MICROPY_HW_NEOPIXEL
static uint8_t status_neopixel_color[3];
static nativeio_digitalinout_obj_t status_neopixel;
#endif

void neopixel_status_init() {
    #ifdef MICROPY_HW_NEOPIXEL
        common_hal_nativeio_digitalinout_construct(&status_neopixel, MICROPY_HW_NEOPIXEL);
        common_hal_nativeio_digitalinout_switch_to_output(&status_neopixel, false, DRIVE_MODE_PUSH_PULL);
    #endif
}

void new_status_color(uint8_t r, uint8_t g, uint8_t b) {
    #ifdef MICROPY_HW_NEOPIXEL
        status_neopixel_color[0] = g;
        status_neopixel_color[1] = r;
        status_neopixel_color[2] = b;
        common_hal_neopixel_write(&status_neopixel, status_neopixel_color, 3, true);
    #endif
}

void temp_status_color(uint8_t r, uint8_t g, uint8_t b) {
    #ifdef MICROPY_HW_NEOPIXEL
        uint8_t colors[3] = {g, r, b};
        common_hal_neopixel_write(&status_neopixel, colors, 3, true);
    #endif
}

void clear_temp_status() {
    #ifdef MICROPY_HW_NEOPIXEL
        common_hal_neopixel_write(&status_neopixel, status_neopixel_color, 3, true);
    #endif
}
