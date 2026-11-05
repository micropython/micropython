// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Brian Nelson
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"

#include "common-hal/microcontroller/Pin.h"
#include "driver/gpio.h"
#include "py/mphal.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/dotclockframebuffer/DotClockFramebuffer.h"
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/settings.h"

#define DELAY 0x80

// TCA9554 I/O expander
#define TCA9554_ADDRESS     (0x20)
#define TCA9554_REG_OUTPUT  (1)
#define TCA9554_REG_CONFIG  (3)

#define EXIO_LCD_RESET      (1 << 0)
#define EXIO_TP_RESET       (1 << 1)
#define EXIO_LCD_CS         (1 << 2)
#define EXIO_SD_CS          (1 << 3)
#define EXIO_BUZZER         (1 << 7)
// IMU INT2/INT1 and RTC INT (bits 4-6) are inputs
#define EXIO_INPUTS         (0x70)

// The ST7701S is configured over 3-wire (9-bit) SPI.
// CS is on the I/O expander, SDA and SCK are shared with the SD card.
#define LCD_SPI_SDA         (GPIO_NUM_1)
#define LCD_SPI_SCK         (GPIO_NUM_2)

#define BACKLIGHT_PIN       (GPIO_NUM_6)

// ST7701S init sequence from the Waveshare example code
static const uint8_t display_init_sequence[] = {
    0xff, 5, 0x77, 0x01, 0x00, 0x00, 0x10,
    0xc0, 2, 0x3b, 0x00,
    0xc1, 2, 0x0b, 0x02,
    0xc2, 2, 0x07, 0x02,
    0xcc, 1, 0x10,
    0xcd, 1, 0x08,
    0xb0, 16, 0x00, 0x11, 0x16, 0x0e, 0x11, 0x06, 0x05, 0x09, 0x08, 0x21, 0x06, 0x13, 0x10, 0x29, 0x31, 0x18,
    0xb1, 16, 0x00, 0x11, 0x16, 0x0e, 0x11, 0x07, 0x05, 0x09, 0x09, 0x21, 0x05, 0x13, 0x11, 0x2a, 0x31, 0x18,
    0xff, 5, 0x77, 0x01, 0x00, 0x00, 0x11,
    0xb0, 1, 0x6d,
    0xb1, 1, 0x37,
    0xb2, 1, 0x81,
    0xb3, 1, 0x80,
    0xb5, 1, 0x43,
    0xb7, 1, 0x85,
    0xb8, 1, 0x20,
    0xc1, 1, 0x78,
    0xc2, 1, 0x78,
    0xd0, 1, 0x88,
    0xe0, 3, 0x00, 0x00, 0x02,
    0xe1, 11, 0x03, 0xa0, 0x00, 0x00, 0x04, 0xa0, 0x00, 0x00, 0x00, 0x20, 0x20,
    0xe2, 13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xe3, 4, 0x00, 0x00, 0x11, 0x00,
    0xe4, 2, 0x22, 0x00,
    0xe5, 16, 0x05, 0xec, 0xa0, 0xa0, 0x07, 0xee, 0xa0, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xe6, 4, 0x00, 0x00, 0x11, 0x00,
    0xe7, 2, 0x22, 0x00,
    0xe8, 16, 0x06, 0xed, 0xa0, 0xa0, 0x08, 0xef, 0xa0, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xeb, 7, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00,
    0xed, 16, 0xff, 0xff, 0xff, 0xba, 0x0a, 0xbf, 0x45, 0xff, 0xff, 0x54, 0xfb, 0xa0, 0xab, 0xff, 0xff, 0xff,
    0xef, 6, 0x10, 0x0d, 0x04, 0x08, 0x3f, 0x1f,
    0xff, 5, 0x77, 0x01, 0x00, 0x00, 0x13,
    0xef, 1, 0x08,
    0xff, 5, 0x77, 0x01, 0x00, 0x00, 0x00,
    0x36, 1, 0x00, // MADCTL
    0x3a, 1, 0x66, // COLMOD
    0x11, DELAY, 0xff, // SLPOUT, delay 500ms
    0x20, DELAY, 120, // INVOFF, delay 120ms
    0x29, 0, // DISPON
};

static const mcu_pin_obj_t *blue_pins[] = {
    &pin_GPIO5, &pin_GPIO45, &pin_GPIO48, &pin_GPIO47, &pin_GPIO21
};
static const mcu_pin_obj_t *green_pins[] = {
    &pin_GPIO14, &pin_GPIO13, &pin_GPIO12, &pin_GPIO11, &pin_GPIO10, &pin_GPIO9
};
static const mcu_pin_obj_t *red_pins[] = {
    &pin_GPIO46, &pin_GPIO3, &pin_GPIO8, &pin_GPIO18, &pin_GPIO17
};

static void expander_write(busio_i2c_obj_t *i2c, uint8_t reg, uint8_t value) {
    const uint8_t buf[] = { reg, value };
    common_hal_busio_i2c_write(i2c, TCA9554_ADDRESS, buf, sizeof(buf));
}

static void lcd_spi_send(bool is_data, uint8_t value) {
    uint16_t bits = value | (is_data ? 0x100 : 0);
    // CPOL=CPHA=0, MSB first: the D/C bit, then 8 data bits
    for (int i = 0; i < 9; i++) {
        gpio_set_level(LCD_SPI_SCK, 0);
        gpio_set_level(LCD_SPI_SDA, (bits & 0x100) != 0);
        gpio_set_level(LCD_SPI_SCK, 1);
        bits <<= 1;
    }
    gpio_set_level(LCD_SPI_SCK, 0);
}

static void send_display_init_sequence(busio_i2c_obj_t *i2c, uint8_t idle_outputs) {
    gpio_set_direction(LCD_SPI_SDA, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_SPI_SCK, GPIO_MODE_OUTPUT);
    gpio_set_level(LCD_SPI_SCK, 0);

    for (size_t i = 0; i < sizeof(display_init_sequence); /* NO INCREMENT */) {
        const uint8_t *cmd = display_init_sequence + i;
        uint8_t data_size = cmd[1] & ~DELAY;
        bool delay = (cmd[1] & DELAY) != 0;
        const uint8_t *data = cmd + 2;

        expander_write(i2c, TCA9554_REG_OUTPUT, idle_outputs & ~EXIO_LCD_CS);
        lcd_spi_send(false, cmd[0]);
        for (uint8_t j = 0; j < data_size; j++) {
            lcd_spi_send(true, data[j]);
        }
        expander_write(i2c, TCA9554_REG_OUTPUT, idle_outputs);

        if (delay) {
            uint16_t delay_ms = data[data_size];
            if (delay_ms == 255) {
                delay_ms = 500;
            }
            mp_hal_delay_ms(delay_ms);
            data_size++;
        }
        i += 2 + data_size;
    }

    // Release the pins for use by the SD card
    gpio_reset_pin(LCD_SPI_SDA);
    gpio_reset_pin(LCD_SPI_SCK);
}

static void display_init(void) {
    busio_i2c_obj_t i2c;
    i2c.base.type = &busio_i2c_type;
    common_hal_busio_i2c_construct(&i2c, &pin_GPIO7, &pin_GPIO15, 400000, 255);
    while (!common_hal_busio_i2c_try_lock(&i2c)) {
    }

    // Hold the LCD and touch controller in reset, everything else idle
    const uint8_t idle_outputs = EXIO_LCD_RESET | EXIO_TP_RESET | EXIO_LCD_CS | EXIO_SD_CS;
    expander_write(&i2c, TCA9554_REG_OUTPUT, idle_outputs & ~(EXIO_LCD_RESET | EXIO_TP_RESET));
    expander_write(&i2c, TCA9554_REG_CONFIG, EXIO_INPUTS);
    mp_hal_delay_ms(10);
    expander_write(&i2c, TCA9554_REG_OUTPUT, idle_outputs);
    mp_hal_delay_ms(120);

    send_display_init_sequence(&i2c, idle_outputs);

    common_hal_busio_i2c_unlock(&i2c);
    common_hal_busio_i2c_deinit(&i2c);

    mp_int_t frequency;
    if (settings_get_int("CIRCUITPY_DISPLAY_FREQUENCY", &frequency) != SETTINGS_OK) {
        frequency = 12000000;
    }

    dotclockframebuffer_framebuffer_obj_t *framebuffer = &allocate_display_bus_or_raise()->dotclock;
    framebuffer->base.type = &dotclockframebuffer_framebuffer_type;
    common_hal_dotclockframebuffer_framebuffer_construct(
        framebuffer,
        &pin_GPIO40,    // de
        &pin_GPIO39,    // vsync
        &pin_GPIO38,    // hsync
        &pin_GPIO41,    // pclk
        red_pins, MP_ARRAY_SIZE(red_pins),
        green_pins, MP_ARRAY_SIZE(green_pins),
        blue_pins, MP_ARRAY_SIZE(blue_pins),
        frequency,      // frequency
        480,            // width
        480,            // height
        8, 10, 50, false, // horiz: pulse, back porch, front porch, idle low
        3, 8, 8, false, // vert: pulse, back porch, front porch, idle low
        false,          // DE idle high
        true,           // pclk active high
        false,          // pclk idle high
        0               // overscan left
        );

    framebufferio_framebufferdisplay_obj_t *display = &allocate_display_or_raise()->framebuffer_display;
    display->base.type = &framebufferio_framebufferdisplay_type;
    common_hal_framebufferio_framebufferdisplay_construct(
        display,
        framebuffer,
        0,              // rotation
        true            // auto-refresh
        );
}

void board_init(void) {
    // BACKLIGHT_PIN controls the backlight. Default to high (on). The pin isn't
    // claimed, so user code can still use it for PWM brightness.
    espressif_board_reset_pin_number(BACKLIGHT_PIN);

    display_init();
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Keep the backlight on across resets, but leave the pin free for PWM brightness control
    if (pin_number == BACKLIGHT_PIN) {
        config_pin_as_output_with_level(pin_number, true);
        return true;
    }
    return false;
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
