// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Bradán Lane STUDIO
//
// SPDX-License-Identifier: MIT

/*
    The Explorer Badge(s) have more than one specific hardware configuration.
    This is a result of small changes in parts availability. The changes are
    insignificant to the end user but require changes to the initialization.
    The hardware revisions use a voltage divider connected to pin GP29 to
    indicate which hardware configuration exists. The code generates a
    "version ID" or VID which is used by the code to perform the correct
    initialization. The VID is also exposed to the end user in case there is
    a need - either for documentation, support requests, or tutorial materials.
*/

#include "mpconfigboard.h"

#include "supervisor/board.h"
#include "shared-bindings/board/__init__.h"

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/board.h"

#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"

#include "src/rp2_common/hardware_adc/include/hardware/adc.h"
#define ADC_FIRST_PIN_NUMBER 26
#define ADC_PIN_COUNT 4
extern void common_hal_mcu_delay_us(uint32_t);

#define HEIGHT      200
#define WIDTH       200

#define DELAY_FLAG  0x80

#define EPD_RAM_BW  0x10
#define EPD_RAM_RED 0x13

#define DISPLAY_EN_PIN 8

// These commands are the combination of SSD1608 and SSD1681 and not all commands are supported for each controller
#define SSD_DRIVER_CONTROL         0x01
#define SSD_GATE_VOLTAGE           0x03
#define SSD_SOURCE_VOLTAGE         0x04
#define SSD_DISPLAY_CONTROL        0x07
#define SSD_PROGOTP_INITIAL        0x08
#define SSD_WRITEREG_INITIAL       0x09
#define SSD_READREG_INITIAL        0x0A
#define SSD_NON_OVERLAP            0x0B
#define SSD_BOOST_SOFT_START       0x0C
#define SSD_GATE_SCAN_START        0x0F
#define SSD_DEEP_SLEEP             0x10
#define SSD_DATA_MODE              0x11
#define SSD_SW_RESET               0x12
#define SSD_HV_DETECT              0x14
#define SSD_VCI_DETECT             0x15
#define SSD_TEMP_CONTROL_1681      0x18
#define SSD_TEMP_CONTROL_1608      0x1C
#define SSD_TEMP_WRITE             0x1A
#define SSD_TEMP_READ              0x1B
#define SSD_TEMP_EXTERN            0x1C
#define SSD_MASTER_ACTIVATE        0x20
#define SSD_DISP_CTRL1             0x21
#define SSD_DISP_CTRL2             0x22
#define SSD_WRITE_RAM_BLK          0x24
#define SSD_READ_RAM_BLK           0x25
#define SSD_WRITE_RAM_RED          0x26
#define SSD_READ_RAM_RED           0x27
#define SSD_VCOM_SENSE             0x28
#define SSD_VCOM_DURRATION         0x29
#define SSD_PROG_VCOM              0x2A
#define SSD_CTRL_VCOM              0x2B
#define SSD_WRITE_VCOM             0x2C
#define SSD_READ_OTP               0x2D
#define SSD_READ_ID                0x2E
#define SSD_READ_STATUS            0x2F
#define SSD_WRITE_LUT              0x32
#define SSD_WRITE_DUMMY            0x3A
#define SSD_WRITE_GATELINE_1608    0x3B
#define SSD_WRITE_BORDER           0x3C
#define SSD_SET_RAMXPOS            0x44
#define SSD_SET_RAMYPOS            0x45
#define SSD_SET_RAMXCOUNT          0x4E
#define SSD_SET_RAMYCOUNT          0x4F
#define SSD_NOP                    0xFF

const uint8_t _start_sequence_ssd1681[] = {
    SSD_SW_RESET,           DELAY_FLAG + 0, 20,                                                         // soft reset and wait 20ms
    SSD_DATA_MODE,          1,              0x03,                                                       // Data entry sequence
    SSD_WRITE_BORDER,       1,              0x05,                                                       // border color
    SSD_TEMP_CONTROL_1681,  1,              0x80,                                                       // Temperature control
    SSD_SET_RAMXCOUNT,      1,              0x00,
    SSD_SET_RAMYCOUNT,      2,              0x00, 0x00,
    SSD_DRIVER_CONTROL,     3,              ((WIDTH - 1) & 0xFF), (((WIDTH >> 8) - 1) & 0xFF), 0x00,    // set display size
    SSD_DISP_CTRL2,         1,              0xf7,                                                       // Set DISP only full refreshes
};
const uint8_t _stop_sequence_ssd1681[] = {
    SSD_DEEP_SLEEP,         DELAY_FLAG + 1, 0x01, 0x64                                                  // Enter deep sleep
};
const uint8_t _refresh_sequence_ssd1681[] = {
    SSD_MASTER_ACTIVATE, 0,
};


const uint8_t _start_sequence_ssd1608[] = {
    SSD_SW_RESET,           DELAY_FLAG + 0, 20,                                                         // soft reset and wait 20ms
    SSD_DRIVER_CONTROL,     3,              ((WIDTH - 1) & 0xFF), (((WIDTH - 1) >> 8) & 0xFF), 0x00,    // set display size
    SSD_WRITE_GATELINE_1608, 1,             0x0B,   // gate line width
    SSD_DATA_MODE,          1,              0x03,                                                       // Data entry sequence
    SSD_WRITE_VCOM,         1,              0x70,
    SSD_WRITE_LUT,         30,              0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 0x66, 0x69,
    0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 0x00, 0x00, 0x00, 0x00,
    0xf8, 0xb4, 0x13, 0x51, 0x35, 0x51, 0x51, 0x19, 0x01, 0x00,
    SSD_DISP_CTRL2,         1,              0xC7,                                                       // Set DISP only full refreshes
};
const uint8_t _stop_sequence_ssd1608[] = {
    SSD_DEEP_SLEEP,         DELAY_FLAG + 1, 0x01, 0x64                                                  // Enter deep sleep
};
const uint8_t _refresh_sequence_ssd1608[] = {
    SSD_MASTER_ACTIVATE, 0,
};


extern uint16_t vid_setting;    // declared in pins.c
// _set_vid() uses the GPIO29 analog pin (which is connected to a voltage divider) to computer a revision code for the board
// this allows multiple similar boards to chare the same CP build
static int _set_vid(void) {
    vid_setting = 9999;

    #define DCK01_VID_PIN 29
    uint16_t value;
    adc_init();
    adc_gpio_init(DCK01_VID_PIN);
    adc_select_input(DCK01_VID_PIN - ADC_FIRST_PIN_NUMBER); // the VID pin is 29 and the first ADC pin is 26
    common_hal_mcu_delay_us(100);
    uint32_t accum = 0;
    for (int i = 0; i < 10; i++) {
        accum += adc_read();
    }
    value = accum / 10; // average the readings
    vid_setting = value;
    /*
        Voltage Divider with 3.3V: (1241 * V)
          10K/ 15K = 1.98V = 2458    GT 2000 = TBD
          15K/ 10K = 1.32V = 1638    GT 1200 = Explorer  with SSD1681 BW
          15K/4.7K = 0.79V =  980    GT  600 = Explorer  with SSD1681 BWR
          15K/  2K = 0.39V =  482    GT  300 = Explorer  with SSD1608 BW
         100K/ 10K = 0.30V =  372              ditto
          15K/  1K = 0.21V =  256    GT  150 = DCNextGen with SSD1681 BWR
        Note: extreme values (using 100K or greater) will not create a strong enough current for the ADC to read accurately
        Note: we do not get a usable value when the voltage divider is missing
    */

    // TODO change to min/max to tighten up the ranges (requires sampling of the initial boards)
    if (value > 2800) {
        vid_setting = 9;            // invalid
    } else if (value > 2000) {
        vid_setting = 5;            // future
    } else if (value > 1200) {
        vid_setting = 4;            // Explorer  SSD1681 BW
    } else if (value > 600) {
        vid_setting = 3;            // Explorer  SSD1681 BWR
    } else if (value > 300) {
        vid_setting = 2;            // Explorer  SSD1608 BW
    } else if (value > 150) {
        vid_setting = 1;            // DCNextGen SSD1681 BWR
    } else {
        vid_setting = 0;            // invalid

    }
    return vid_setting;
}


// Note: board_reset_pin_number() is new to CP9 and allows a board to handle pin resets on a per-pin basis
// we use it to prevent the DISPLAY_EN pin from automatically changing state
bool board_reset_pin_number(uint8_t pin_number) {
    static bool _display_pin_inited = false;

    if (pin_number == DISPLAY_EN_PIN) {
        // init the pin the first time; do nothing after that
        if (!_display_pin_inited) {
            _display_pin_inited = true;
            // doing this (rather than gpio_init) in this specific order ensures no
            // glitch if pin was already configured as a high output. gpio_init() temporarily
            // configures the pin as an input, so the power enable value would potentially
            // glitch.
            gpio_put(pin_number, 1);
            gpio_set_dir(pin_number, GPIO_OUT);
            hw_write_masked(&pads_bank0_hw->io[pin_number], PADS_BANK0_GPIO0_DRIVE_VALUE_12MA << PADS_BANK0_GPIO0_DRIVE_LSB, PADS_BANK0_GPIO0_DRIVE_BITS);
            gpio_set_function(pin_number, GPIO_FUNC_SIO);
        }
        return true;
    }
    return false;
}

void board_init(void) {
    board_reset_pin_number(DISPLAY_EN_PIN);
    _set_vid(); // sets vid_setting global

    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO14, &pin_GPIO15, NULL, false);
    common_hal_busio_spi_never_reset(spi);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        &pin_GPIO11, // DEFAULT_SPI_BUS_DC,                                                             // EPD_DC Command or data
        &pin_GPIO13, // DEFAULT_SPI_BUS_CS,                                                             // EPD_CS Chip select
        &pin_GPIO10, // DEFAULT_SPI_BUS_RESET,                                                          // EPD_RST Reset
        1000000,                                                                                        // Baudrate
        0,                                                                                              // Polarity
        0);                                                                                             // Phase

    // board_vid is a computed flag to let us know what hardware is active
    // currently, we only know two codes '1' and '2' and these indicate what ePaper display is installed

    epaperdisplay_epaperdisplay_obj_t *display = NULL;

    // Set up the DisplayIO epaper object
    display = &allocate_display()->epaper_display;
    display->base.type = &epaperdisplay_epaperdisplay_type;

    // default to no rotation
    int rotation = 0;
    if (vid_setting == 1) {
        // DCNextGen SSD1681 BWR rotated 270
        rotation = 270;
    }

    // default to BWR refresh rates
    float refresh_time = 15.0;
    float seconds_per_frame = 20.0;
    if ((vid_setting == 2) || (vid_setting == 4)) {
        // BW displays have faster refresh rates
        refresh_time = 1.0;
        seconds_per_frame = 5.0;
    }

    // VID 1, 3, and 4 = SSD1681 display driver
    // VID 2           = SSD1608 display driver

    // VID codes: see above
    if ((vid_setting == 1) ||   // DCNextGen SSD1681 BWR rotated 270
        (vid_setting == 3) ||   // Explorer  SSD1681 BW  rotated 0
        (vid_setting == 4)) {   // Explorer  SSD1681 BWR rotated 0
        common_hal_epaperdisplay_epaperdisplay_construct(
            display,
            bus,
            _start_sequence_ssd1681, sizeof(_start_sequence_ssd1681),
            1.0,         // start up time
            _stop_sequence_ssd1681, sizeof(_stop_sequence_ssd1681),
            WIDTH,                                                                                          // width
            HEIGHT,                                                                                         // height
            WIDTH,                                                                                          // ram_width
            HEIGHT + 0x60,                                                                                  // ram_height RAM is actually only 200 bits high but we use 296 to match the 9 bits
            0,                                                                                              // colstart
            0,                                                                                              // rowstart
            rotation,                                                                                       // rotation
            SSD_SET_RAMXPOS,                                                                                // set_column_window_command
            SSD_SET_RAMYPOS,                                                                                // set_row_window_command
            SSD_SET_RAMXCOUNT,                                                                              // set_current_column_command
            SSD_SET_RAMYCOUNT,                                                                              // set_current_row_command
            SSD_WRITE_RAM_BLK,                                                                              // write_black_ram_command
            false,                                                                                          // black_bits_inverted
            SSD_WRITE_RAM_RED,                                                                              // write_color_ram_command
            false,                                                                                          // color_bits_inverted
            0xFF0000,                                                                                       // highlight_color (RED for tri-color display)
            _refresh_sequence_ssd1681, sizeof(_refresh_sequence_ssd1681),                                   // refresh_display_command
            refresh_time,                                                                                   // refresh_time
            &pin_GPIO9, // DEFAULT_SPI_BUS_BUSY,                                                            // busy_pin
            true,                                                                                           // busy_state
            seconds_per_frame,                                                                              // seconds_per_frame (does not seem the user can change this)
            true,                                                                                           // always_toggle_chip_select
            false,                                                                                          // not grayscale
            false,                                                                                          // not acep
            false,                                                                                          // not two_byte_sequence_length
            true);                                                                                          // address_little_endian
    } else if (vid_setting == 2) {  // Explorer SSD1608 BW
        common_hal_epaperdisplay_epaperdisplay_construct(
            display,
            bus,
            _start_sequence_ssd1608, sizeof(_start_sequence_ssd1608),
            1.0,         // start up time
            _stop_sequence_ssd1608, sizeof(_stop_sequence_ssd1608),
            WIDTH,                                                                                          // width
            HEIGHT,                                                                                         // height
            WIDTH,                                                                                          // ram_width
            HEIGHT /* + 0x60 */,                                                                             // ram_height RAM is actually only 200 bits high but we use 296 to match the 9 bits
            0,                                                                                              // colstart
            0,                                                                                              // rowstart
            rotation,                                                                                       // rotation
            SSD_SET_RAMXPOS,                                                                                // set_column_window_command
            SSD_SET_RAMYPOS,                                                                                // set_row_window_command
            SSD_SET_RAMXCOUNT,                                                                              // set_current_column_command
            SSD_SET_RAMYCOUNT,                                                                              // set_current_row_command
            SSD_WRITE_RAM_BLK,                                                                              // write_black_ram_command
            false,                                                                                          // black_bits_inverted
            NO_COMMAND,                                                                                     // write_color_ram_command
            false,                                                                                          // color_bits_inverted
            0x000000,                                                                                       // highlight_color (RED for tri-color display)
            _refresh_sequence_ssd1608, sizeof(_refresh_sequence_ssd1608),                                   // refresh_display_command
            refresh_time,                                                                                   // refresh_time
            &pin_GPIO9, // DEFAULT_SPI_BUS_BUSY,                                                            // busy_pin
            true,                                                                                           // busy_state
            seconds_per_frame,                                                                              // seconds_per_frame (does not seem the user can change this)
            true,                                                                                           // always_toggle_chip_select
            false,                                                                                          // not grayscale
            false,                                                                                          // not acep
            false,                                                                                          // not two_byte_sequence_length
            true);                                                                                          // address_little_endian
    } else {
        // what should happen if this firmware is installed on some other board?
        // currently, we mark the display as None
        display->base.type = &mp_type_NoneType;
    }

}

void board_deinit(void) {
    if ((vid_setting == 1) || (vid_setting == 2)) {
        // we initialized an ePaper display so we can de-init it
        epaperdisplay_epaperdisplay_obj_t *display = &displays[0].epaper_display;
        if (display->base.type == &epaperdisplay_epaperdisplay_type) {
            while (common_hal_epaperdisplay_epaperdisplay_get_busy(display)) {
                // RUN_BACKGROUND_TASKS;
            }
        }
        common_hal_displayio_release_displays();
    }
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
