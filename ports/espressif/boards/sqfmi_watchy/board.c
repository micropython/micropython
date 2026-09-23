// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/board.h"
#include "driver/gpio.h"

#include "common-hal/microcontroller/Pin.h"

#include "esp_log.h"
#include "supervisor/filesystem.h"

#define DELAY               0x80

#define EPD_WIDTH           200
#define EPD_HEIGHT          200
#define EPD_X_OFFSET        0
#define EPD_Y_OFFSET        0
#define EPD_FIRST_COLUMN    EPD_X_OFFSET
#define EPD_LAST_COLUMN     (EPD_X_OFFSET + EPD_WIDTH - 1)
#define EPD_FIRST_ROW       EPD_Y_OFFSET
#define EPD_LAST_ROW        (EPD_Y_OFFSET + EPD_HEIGHT - 1)
#define EPD_X_WINDOW_START  (EPD_FIRST_COLUMN / 8)
#define EPD_X_WINDOW_END    (EPD_LAST_COLUMN / 8)
#define EPD_Y_WINDOW_START  EPD_FIRST_ROW
#define EPD_Y_WINDOW_END    EPD_LAST_ROW



#define EPD_EVEN_FIRST      0x0
#define EPD_ODD_FIRST       0x4
#define EPD_NO_INTERLACE    0x0
#define EPD_INTERLACE       0x2
#define EPD_SCAN_FORWARD    0x0
#define EPD_SCAN_BACKWARD   0x1

#define EPD_WHITE_BORDER    0x5
#define EPD_BLACK_BORDER    0x2

#define EPD_EXTERNAL_SENSOR 0x48
#define EPD_INTERNAL_SENSOR 0x80

#define EPD_RED_ENABLE      0x00
#define EPD_RED_DISABLE     0x40
#define EPD_RED_INVERT      0x80
#define EPD_BLACK_ENABLE    0x00
#define EPD_BLACK_DISABLE   0x04
#define EPD_BLACK_INVERT    0x08

#define EPD_Y_FORWARDS      0x02
#define EPD_Y_BACKWARDS     0x00
#define EPD_X_FORWARDS      0x01
#define EPD_X_BACKWARDS     0x00
#define EPD_RIGHT_THEN_DOWN 0x00
#define EPD_DOWN_THEN_RIGHT 0x04

#define EPD_MODE_NORMAL     0x00
#define EPD_MODE_SLEEP_1    0x01
#define EPD_MODE_SLEEP_2    0x03

// Combine the below commands to make an update sequence.
// Note that commands like CLOCK_ON and CLOCK_OFF can be used together.
// In this case, the signal will be enabled before the update and disabled after.
#define EPDU_CLOCK_ON       0x80
#define EPDU_ANALOG_ON      0x40
#define EPDU_LOAD_TEMP      0x20
#define EPDU_LOAD_LUT       0x10
#define EPDU_DISP_MODE_1    0x00
#define EPDU_DISP_MODE_2    0x08
#define EPDU_OUTPUT         0x04
#define EPDU_ANALOG_OFF     0x02
#define EPDU_CLOCK_OFF      0x01
#define EPDU_DISPLAY        (EPDU_OUTPUT | EPDU_LOAD_LUT)

// Watchy EPD settings
#define EPD_GATE_SETTINGS   EPD_EVEN_FIRST | EPD_NO_INTERLACE | EPD_SCAN_FORWARD
#define EPD_ENTRY_MODE      EPD_X_FORWARDS | EPD_Y_FORWARDS | EPD_RIGHT_THEN_DOWN
#define EPD_COLOUR_MODE     EPD_RED_DISABLE | EPD_BLACK_INVERT
#define EPD_UPDATE_SEQUENCE (EPDU_CLOCK_ON | EPDU_ANALOG_ON | EPDU_LOAD_TEMP | \
    EPDU_DISPLAY | EPDU_DISP_MODE_1 | EPDU_CLOCK_OFF | \
    EPDU_ANALOG_OFF)
#define EPD_POST_UPDATE     (EPDU_CLOCK_ON | EPDU_CLOCK_OFF | EPDU_ANALOG_OFF)


#define EPDCMD_GATE_SETTING         0x01
#define EPDCMD_DEEP_SLEEP           0x10
#define EPDCMD_ENTRY_MODE           0x11
#define EPDCMD_SOFT_RESET           0x12
#define EPDCMD_TEMP_SENSOR          0x18
#define EPDCMD_PANEL_COLOURS        0x21
#define EPDCMD_MASTER_ACTIVATE      0x20
#define EPDCMD_UPDATE_SEQUENCE      0x22
#define EPDCMD_BORDER_WAVEFORM      0x3C
#define EPDCMD_WRITE_RAM            0x24
#define EPDCMD_X_WINDOW             0x44
#define EPDCMD_Y_WINDOW             0x45
#define EPDCMD_X_OFFSET             0x4E
#define EPDCMD_Y_OFFSET             0x4F
#define EPDCMD_NOP                  0x7F


const uint8_t display_start_sequence[] = {
    EPDCMD_SOFT_RESET,      DELAY,  20,
    EPDCMD_GATE_SETTING,    3,      EPD_LAST_ROW % 256, EPD_LAST_ROW / 256,
    EPD_GATE_SETTINGS,
    EPDCMD_BORDER_WAVEFORM, 1,      EPD_BLACK_BORDER,
    EPDCMD_TEMP_SENSOR,     1,      EPD_INTERNAL_SENSOR,
    EPDCMD_PANEL_COLOURS,   1,      EPD_COLOUR_MODE,
    EPDCMD_ENTRY_MODE,      1,      EPD_ENTRY_MODE,
    EPDCMD_X_WINDOW, 2, EPD_X_WINDOW_START, EPD_X_WINDOW_END,
    EPDCMD_Y_WINDOW, 4, EPD_Y_WINDOW_START % 256, EPD_Y_WINDOW_START / 256,
    EPD_Y_WINDOW_END % 256, EPD_Y_WINDOW_END / 256,
    EPDCMD_X_OFFSET, 1, EPD_X_WINDOW_START,
    EPDCMD_Y_OFFSET, 2, EPD_Y_WINDOW_START % 256, EPD_Y_WINDOW_START / 256,

    EPDCMD_UPDATE_SEQUENCE, 1, EPD_UPDATE_SEQUENCE,
};

const uint8_t display_stop_sequence[] = {
    EPDCMD_DEEP_SLEEP, 1, 0x01
};

const uint8_t refresh_sequence[] = {
    EPDCMD_MASTER_ACTIVATE, 0
};



void board_init(void) {
    // Debug UART
    #ifdef DEBUG
    #endif /* DEBUG */

    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO18, &pin_GPIO23, NULL, false);
    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO10), // EPD_DC Command or data
        MP_OBJ_FROM_PTR(&pin_GPIO5), // EPD_CS Chip select
        MP_OBJ_FROM_PTR(&pin_GPIO9), // EPD_RST Reset
        2000000, // Baudrate
        0, // Polarity
        0 // Phase
        );

    epaperdisplay_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &epaperdisplay_epaperdisplay_type;

    epaperdisplay_construct_args_t args = EPAPERDISPLAY_CONSTRUCT_ARGS_DEFAULTS;
    args.bus = bus;
    args.start_sequence = display_start_sequence;
    args.start_sequence_len = sizeof(display_start_sequence);
    args.start_up_time = 0.01;
    args.stop_sequence = display_stop_sequence;
    args.stop_sequence_len = sizeof(display_stop_sequence);
    args.width = 200;
    args.height = 200;
    args.ram_width = 200;
    args.ram_height = 296;
    args.set_column_window_command = 0x44;
    args.set_row_window_command = 0x45;
    args.set_current_column_command = 0x4E;
    args.set_current_row_command = 0x4F;
    args.write_black_ram_command = 0x24;
    args.black_bits_inverted = true;
    args.write_color_ram_command = 0x26;
    args.refresh_sequence = refresh_sequence;
    args.refresh_sequence_len = sizeof(refresh_sequence);
    args.refresh_time = 1.0;
    args.busy_pin = &pin_GPIO19;
    args.busy_state = true;
    args.seconds_per_frame = 5.0;
    args.address_little_endian = true;
    common_hal_epaperdisplay_epaperdisplay_construct(display, &args);
}

bool board_requests_safe_mode(void) {
    return false;
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    if (pin_number == 13) {
        config_pin_as_output_with_level(pin_number, false);
        return true;
    }
    return false;
}

void reset_board(void) {
    config_pin_as_output_with_level(13, false);
}

void board_deinit(void) {
    config_pin_as_output_with_level(13, false);
}
