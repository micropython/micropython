#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

#include "common-hal/microcontroller/Pin.h"

// reference: lv_micropython ili9XXX.py
// reference: ILI9488 datasheet

#define DELAY_FLAG 0x80

#define COLORMODE_BGR 0b00001000
#define ROTATION_Y_FLIP 0b10000000
#define ROTATION_X_FLIP 0b01000000
#define ROTATION_MV 0b00100000

// DBI type C (SPI) only has 3bit and 18bit format support, 3bit = 2 pixels per byte, 18bit = one color per byte
#define COLORFORMAT_3BIT 0b00000001
#define COLORFORMAT_16BIT 0b00000101
#define COLORFORMAT_18BIT 0b00000110
#define COLORFORMAT_24BIT 0b00000111

static uint8_t display_init_sequence[] = {
    0x01, DELAY_FLAG | 0, 200, // Software Reset
    0x11, DELAY_FLAG | 0, 120, // Exit Sleep Mode
    0xE0, 15, 0x00, 0x03, 0x09, 0x08, 0x16, 0x0A, 0x3F, 0x78, 0x4C, 0x09, 0x0A, 0x08, 0x16, 0x1A, 0x0F, // Positive Gamma Control
    0xE1, 15, 0x00, 0x16, 0x19, 0x03, 0x0F, 0x05, 0x32, 0x45, 0x46, 0x04, 0x0E, 0x0D, 0x35, 0x37, 0x0F, // Negative Gamma Control
    0xC0, 2, 0x17, 0x15,   // Power Control 1
    0xC1, 1, 0x41,         // Power Control 2
    0xC2, 1, 0x44,         // Power Control 3 / Normal Mode
    0xC5, 3, 0x00, 0x12, 0x80, // VCOM Control
    0x36, 1, ROTATION_Y_FLIP,  // Colormode & Rotation
    0x3A, 1, COLORFORMAT_18BIT, // Interface pixel format
    0xB0, 1, 0x00,         // Interface mode control
    0xB1, 1, 0xA0,         // Frame Rate Control
    0xB4, 1, 0x02,         // Display Inversion Control
    0xB6, 2, 0x02, 0x02,   // Display Function Control
    0xE9, 1, 0x00,         // Set Image Function
    0x53, 1, 0x28,         // CTRL Display Value
    0x51, 1, 0x7F,         // Display Brightness
    0xF7, 4, 0xA9, 0x51, 0x2C, 0x02, // Adjust Control 3
    0x29, DELAY_FLAG | 0, 25 // Display ON
};

void board_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO14, &pin_GPIO13, NULL, false);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO2),  // TFT_DC Command or data
        MP_OBJ_FROM_PTR(&pin_GPIO15), // TFT_CS Chip select
        mp_const_none,                // TFT_RST Reset
        20000000, // Baudrate
        0, // Polarity
        0); // Phase

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;
    common_hal_busdisplay_busdisplay_construct(display,
        bus,
        320, // Width (after rotation)
        480, // Height (after rotation)
        0, // column start
        0, // row start
        0, // rotation
        24, // Color depth
        false, // grayscale
        false, // pixels in byte share row. only used for depth < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // Set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS, // Set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // Write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO27,  // backlight pin
        0x51, // cmd to write brightness
        0.5f, // brightness
        false, // single_byte_bounds
        false, // data_as_commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false, // SH1107_addressing
        50000); // backlight pwm frequency
}

void board_deinit(void) {
    common_hal_displayio_release_displays();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
