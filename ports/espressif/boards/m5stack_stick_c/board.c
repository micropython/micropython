// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 CDarius
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "components/driver/gpio/include/driver/gpio.h"
#include "components/hal/include/hal/gpio_hal.h"
#include "common-hal/microcontroller/Pin.h"

#include "../../pmic/axp192/axp192.h"

// display init sequence according to adafruit_st7735r.py library
uint8_t display_init_sequence[] = {
    0x01, 0x80, 0x96,             // SWRESET and Delay 150ms
    0x11, 0x80, 0xff,             // SLPOUT and Delay
    0xb1, 0x03, 0x01, 0x2C, 0x2D,   // _FRMCTR1
    0xb2, 0x03, 0x01, 0x2C, 0x2D,   // _FRMCTR2
    0xb3, 0x06, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D, // _FRMCTR3
    0xb4, 0x01, 0x07,             // _INVCTR line inversion
    0xc0, 0x03, 0xa2, 0x02, 0x84,   // _PWCTR1 GVDD = 4.7V, 1.0uA
    0xc1, 0x01, 0xc5,             // _PWCTR2 VGH=14.7V, VGL=-7.35V
    0xc2, 0x02, 0x0a, 0x00,        // _PWCTR3 Opamp current small, Boost frequency
    0xc3, 0x02, 0x8a, 0x2a,
    0xc4, 0x02, 0x8a, 0xee,
    0xc5, 0x01, 0x0e,             // _VMCTR1 VCOMH = 4V, VOML = -1.1V
    0x36, 0x01, 0xc8,             // MADCTL Rotate display
    0x21, 0x00,                  // _INVON
    0x3a, 0x01, 0x05,             // COLMOD - 16bit color
    0xe0, 0x10, 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10, // _GMCTRP1 Gamma
    0xe1, 0x10, 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10, // _GMCTRN1
    0x13, 0x80, 0x0a,             // _NORON
    0x29, 0x80, 0x64              // _DISPON
};

static bool pmic_init(busio_i2c_obj_t *i2c) {
    int rc;
    uint8_t write_buf[2];

    if (!pmic_common_init(i2c)) {
        return false;
    }

    // Reg: 30h
    // The VBUS-IPSOUT path can be selected to be opened regardless of the status of N_VBUSEN
    // VBUS VHOLD pressure limit control disabled
    // VBUS current limit control disabled
    write_buf[0] = AXP192_VBUS_IPSOUT;
    write_buf[1] = AXP192_VBUS_IPSOUT_IGNORE_VBUSEN;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 33h
    // Charge function enable control bit, including internal and external channels
    // Charging target voltage: 4.2V
    // Charging end current: End charging when charging current is less than 10% setting
    // Internal path charging current: 100mA
    write_buf[0] = AXP192_CHARGING_CTRL1;
    write_buf[1] = AXP192_CHARGING_CTRL1_ENABLE |
        AXP192_CHARGING_CTRL1_VOLTAGE_4_20V |
        AXP192_CHARGING_CTRL1_CURRENT_100mA;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 90h
    // GPIO0(LDOio0) floating
    write_buf[0] = AXP192_GPIO0_FUNCTION;
    write_buf[1] = AXP192_GPIO0_FUNCTION_FLOATING;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 91h
    // GPIO0(LDOio0) 2.8V
    write_buf[0] = AXP192_GPIO0_LDO_VOLTAGE;
    write_buf[1] = AXP192_GPIO0_LDO_VOLTAGE_2_8V;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 28h
    // LDO2 (TFT backlight): 2.8V
    // LDO3 (TFT logic): 3.0V
    write_buf[0] = AXP192_LDO23_OUT_VOLTAGE;
    write_buf[1] = AXP192_LDO23_OUT_VOLTAGE_LDO2_2_8V |
        AXP192_LDO23_OUT_VOLTAGE_LDO3_3_0V;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 12h
    // Enable CTRL_EXTEN, DCDC1, LDO2 and LDO3
    write_buf[0] = AXP192_DCDC13_LDO23_CTRL;
    write_buf[1] = AXP192_DCDC13_LDO23_CTRL_EXTEN |
        AXP192_DCDC13_LDO23_CTRL_LDO3 |
        AXP192_DCDC13_LDO23_CTRL_LDO2 |
        AXP192_DCDC13_LDO23_CTRL_DCDC1;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 26h
    // DCDC1 (ESP32 VDD): 3.350V
    write_buf[0] = AXP192_DCDC1_OUT_VOLTAGE;
    write_buf[1] = AXP192_DCDC1_OUT_VOLTAGE_3_350V;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    if (!pmic_disable_all_irq(i2c)) {
        return false;
    }

    if (!pmic_clear_all_irq(i2c)) {
        return false;
    }

    if (!pmic_enable_power_key_press_irq(i2c)) {
        return false;
    }

    if (!pmic_enable_low_battery_irq(i2c)) {
        return false;
    }

    return true;
}

static bool display_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO13, &pin_GPIO15, NULL, false);
    common_hal_busio_spi_never_reset(spi);

    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        &pin_GPIO23,    // DC
        &pin_GPIO5,     // CS
        &pin_GPIO18,    // RST
        10000000,       // baudrate
        0,              // polarity
        0               // phase
        );

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        80,             // width (after rotation)
        160,            // height (after rotation)
        26,             // column start
        1,              // row start
        0,              // rotation
        16,             // color depth
        false,          // grayscale
        false,          // pixels in a byte share a row. Only valid for depths < 8
        1,              // bytes per cell. Only valid for depths < 8
        false,          // reverse_pixels_in_byte. Only valid for depths < 8
        true,           // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS,   // set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        NULL,    // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f,           // brightness
        false,          // single_byte_bounds
        false,          // data_as_commands
        true,           // auto_refresh
        80,             // native_frames_per_second
        false,          // backlight_on_high
        false,          // SH1107_addressing
        50000           // backlight pwm frequency
        );

    return true;
}

void board_init(void) {
    busio_i2c_obj_t *internal_i2c = common_hal_board_create_i2c(0);

    if (!pmic_init(internal_i2c)) {
        mp_printf(&mp_plat_print, "could not initialize axp192 pmic\n");
        return;
    }

    if (!display_init()) {
        mp_printf(&mp_plat_print, "could not initialize the display");
        return;
    }
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Set IR led gpio high to prevent power drain from the led
    if (pin_number == 9) {
        gpio_set_direction(pin_number, GPIO_MODE_DEF_OUTPUT);
        gpio_set_level(pin_number, true);
        return true;
    }
    return false;
}
