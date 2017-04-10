#include "asf/common2/services/delay/delay.h"
#include "asf/sam0/drivers/port/port.h"

#include "mphalport.h"

#include "shared-bindings/bitbangio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/neopixel_write/__init__.h"
#include "shared-module/bitbangio/types.h"
#include "rgb_led_status.h"
#include "samd21_pins.h"

#ifdef MICROPY_HW_NEOPIXEL
static uint8_t status_neopixel_color[3];
static digitalio_digitalinout_obj_t status_neopixel;
#endif

#if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
static uint8_t status_apa102_color[12] = {0, 0, 0, 0, 0xff, 0, 0, 0};
static bitbangio_spi_obj_t status_apa102;
#endif

#if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK))
static uint32_t current_status_color = 0;
#endif

void rgb_led_status_init() {
    #ifdef MICROPY_HW_NEOPIXEL
        common_hal_digitalio_digitalinout_construct(&status_neopixel, MICROPY_HW_NEOPIXEL);
        common_hal_digitalio_digitalinout_switch_to_output(&status_neopixel, false, DRIVE_MODE_PUSH_PULL);
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
        shared_module_bitbangio_spi_construct(&status_apa102,
                                              MICROPY_HW_APA102_SCK,
                                              MICROPY_HW_APA102_MOSI,
                                              NULL);
        shared_module_bitbangio_spi_try_lock(&status_apa102);
        shared_module_bitbangio_spi_configure(&status_apa102, 100000, 0, 1, 8);
    #endif
}

void new_status_color(uint32_t rgb) {
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK))
    if (current_status_color == rgb) {
        return;
    }
    current_status_color = rgb;
    #endif

    #ifdef MICROPY_HW_NEOPIXEL
        status_neopixel_color[0] = (rgb >> 8) & 0xff;
        status_neopixel_color[1] = (rgb >> 16) & 0xff;
        status_neopixel_color[2] = rgb & 0xff;
        common_hal_neopixel_write(&status_neopixel, status_neopixel_color, 3);
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
        status_apa102_color[5] = rgb & 0xff;
        status_apa102_color[6] = (rgb >> 8) & 0xff;
        status_apa102_color[7] = (rgb >> 16) & 0xff;
        shared_module_bitbangio_spi_write(&status_apa102, status_apa102_color, 8);
    #endif
}

void temp_status_color(uint32_t rgb) {
    #ifdef MICROPY_HW_NEOPIXEL
        uint8_t colors[3] = {(rgb >> 8) & 0xff, (rgb >> 16) & 0xff, rgb & 0xff};
        common_hal_neopixel_write(&status_neopixel, colors, 3);
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
        uint8_t colors[12] = {0, 0, 0, 0, 0xff, rgb & 0xff, (rgb >> 8) & 0xff, (rgb >> 16) & 0xff, 0x0, 0x0, 0x0, 0x0};
        shared_module_bitbangio_spi_write(&status_apa102, colors, 8);
    #endif
}

void clear_temp_status() {
    #ifdef MICROPY_HW_NEOPIXEL
        common_hal_neopixel_write(&status_neopixel, status_neopixel_color, 3);
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
        shared_module_bitbangio_spi_write(&status_apa102, status_apa102_color, 12);
    #endif
}

uint32_t color_brightness(uint32_t color, uint8_t brightness) {
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK))
    uint32_t result = ((color & 0xff0000) * brightness / 255) & 0xff0000;
    result += ((color & 0xff00) * brightness / 255) & 0xff00;
    result += ((color & 0xff) * brightness / 255) & 0xff;
    return result;
    #else
    return color;
    #endif
}
