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
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "components/driver/gpio/include/driver/gpio.h"
#include "components/hal/include/hal/gpio_hal.h"
#include "common-hal/microcontroller/Pin.h"

#include "../../pmic/axp192/axp192.h"


#define DELAY 0x80
#define PMIC_POWER_SOURCE_USB   0
#define PMIC_POWER_SOURCE_M_BUS 1

// display init sequence according to M5Gfx
uint8_t display_init_sequence[] = {
    0x01, DELAY, 0x80,            // Software reset then delay 0x80 (128ms)
    0xC8, 0x03, 0xFF, 0x93, 0x42,   // Turn on the external command
    0xC0, 0x02, 0x12, 0x12,       // Power Control 1
    0xC1, 0x01, 0x03,             // Power Control 2
    0xC5, 0x01, 0xF2,             // VCOM Control 1
    0xB0, 0x01, 0xE0,             // RGB Interface SYNC Mode
    0xF6, 0x03, 0x01, 0x00, 0x00, // Interface control
    0XE0, 0x0F, 0x00, 0x0C, 0x11, 0x04, 0x11, 0x08, 0x37, 0x89, 0x4C, 0x06, 0x0C, 0x0A, 0x2E, 0x34, 0x0F,   // Positive Gamma Correction
    0xE1, 0x0F, 0x00, 0x0B, 0x11, 0x05, 0x13, 0x09, 0x33, 0x67, 0x48, 0x07, 0x0E, 0x0B, 0x2E, 0x33, 0x0F,   // Negative Gamma Correction
    0xB6, 0x04, 0x08, 0x82, 0x1D, 0x04,  // Display Function Control
    0x3A, 0x01, 0x55,             // COLMOD: Pixel Format Set 16 bit
    0x21, 0x00,                  // Display inversion ON
    0x36, 0x01, 0x08,             // Memory Access Control: RGB order
    0x11, DELAY, 0x78,            // Exit Sleep then delay 0x78 (120ms)
    0x29, DELAY, 0x78,            // Display on then delay 0x78 (120ms)
};

static bool pmic_set_power_source(uint8_t source, busio_i2c_obj_t *i2c) {
    int rc;
    uint8_t read_buf[1];
    uint8_t write_buf[2];

    switch (source)
    {
        case PMIC_POWER_SOURCE_USB:
            // Set GPIO to 3.3V (when LDO OUTPUT mode is active)
            write_buf[0] = AXP192_GPIO0_LDO_VOLTAGE;
            rc = common_hal_busio_i2c_write_read(i2c, AXP192_I2C_ADDRESS, write_buf, 1, read_buf, sizeof(read_buf));
            if (rc != 0) {
                return false;
            }
            write_buf[1] = (read_buf[0] & ~AXP192_GPIO0_LDO_VOLTAGE_MASK) | AXP192_GPIO0_LDO_VOLTAGE_3_3V;
            rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
            if (rc != 0) {
                return false;
            }
            // Set GPIO0 to LDO_OUTPUT to set N_VBUSEN high
            // When N_VBUSEN is high IPSOUT do not select VBUS as source (BUS_5V)
            write_buf[0] = AXP192_GPIO0_FUNCTION;
            rc = common_hal_busio_i2c_write_read(i2c, AXP192_I2C_ADDRESS, write_buf, 1, read_buf, sizeof(read_buf));
            if (rc != 0) {
                return false;
            }
            write_buf[1] = (read_buf[0] & ~AXP192_GPIO0_FUNCTION_MASK) | AXP192_GPIO0_FUNCTION_LDO_OUTPUT;
            rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
            if (rc != 0) {
                return false;
            }

            #if M5STACK_CORE2_5V_OUTPUT_ENABLE_DEFAULT
            // Set EXTENT output high to enable 5V power boost
            write_buf[0] = AXP192_DCDC13_LDO23_CTRL;
            rc = common_hal_busio_i2c_write_read(i2c, AXP192_I2C_ADDRESS, write_buf, 1, read_buf, sizeof(read_buf));
            if (rc != 0) {
                return false;
            }
            write_buf[1] = read_buf[0] | AXP192_DCDC13_LDO23_CTRL_EXTEN;
            rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
            if (rc != 0) {
                return false;
            }
            #endif

            // Enable VBUS-IPSOUT when N_VBUSEN is high
            write_buf[0] = AXP192_VBUS_IPSOUT;
            rc = common_hal_busio_i2c_write_read(i2c, AXP192_I2C_ADDRESS, write_buf, 1, read_buf, sizeof(read_buf));
            if (rc != 0) {
                return false;
            }
            write_buf[1] = read_buf[0] & ~AXP192_VBUS_IPSOUT_IGNORE_VBUSEN;
            rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
            if (rc != 0) {
                return false;
            }

            return true;

        case PMIC_POWER_SOURCE_M_BUS:
            // Enable VBUS-IPSOUT regardless of f N_VBUSEN
            write_buf[0] = AXP192_VBUS_IPSOUT;
            rc = common_hal_busio_i2c_write_read(i2c, AXP192_I2C_ADDRESS, write_buf, 1, read_buf, sizeof(read_buf));
            if (rc != 0) {
                return false;
            }
            write_buf[1] = read_buf[0] | AXP192_VBUS_IPSOUT_IGNORE_VBUSEN;
            rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
            if (rc != 0) {
                return false;
            }

            // Set EXTENT output low to disable 5V power boost
            write_buf[0] = AXP192_DCDC13_LDO23_CTRL;
            rc = common_hal_busio_i2c_write_read(i2c, AXP192_I2C_ADDRESS, write_buf, 1, read_buf, sizeof(read_buf));
            if (rc != 0) {
                return false;
            }
            write_buf[1] = read_buf[0] & ~AXP192_DCDC13_LDO23_CTRL_EXTEN;
            rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
            if (rc != 0) {
                return false;
            }

            // Set GPIO0 to float and the pull down resistor set N_VBUSEN low
            // When N_VBUSEN is low IPSOUT select VBUS as source (BUS_5V)
            write_buf[0] = AXP192_GPIO0_FUNCTION;
            rc = common_hal_busio_i2c_write_read(i2c, AXP192_I2C_ADDRESS, write_buf, 1, read_buf, sizeof(read_buf));
            if (rc != 0) {
                return false;
            }
            write_buf[1] = (read_buf[0] & ~AXP192_GPIO0_FUNCTION_MASK) | AXP192_GPIO0_FUNCTION_FLOATING;
            rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
            if (rc != 0) {
                return false;
            }

            return true;

        default:
            return false;
    }
}

static bool pmic_init(busio_i2c_obj_t *i2c) {
    int rc;
    uint8_t read_buf[1];
    uint8_t write_buf[2];

    if (!pmic_common_init(i2c)) {
        return false;
    }

    // Reg: 30h
    // The VBUS-IPSOUT path can be selected to be opened regardless of the status of N_VBUSEN
    // VBUS VHOLD pressure limit control disabled
    // VBUS current limit control enabled
    write_buf[0] = AXP192_VBUS_IPSOUT;
    write_buf[1] = AXP192_VBUS_IPSOUT_VBUS_LIMIT_CURRENT |
        AXP192_VBUS_IPSOUT_VBUS_LIMIT_CURRENT_500mA;
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
        AXP192_CHARGING_CTRL1_CHARGING_THRESH_10PERC |
        AXP192_CHARGING_CTRL1_CURRENT_360mA;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 98h
    // PWM1 frequency
    write_buf[0] = AXP192_PWM1_OUTPUT_FREQUECY;
    write_buf[1] = 0x00;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 99h
    // PWM1 duty cycle Y1
    write_buf[0] = AXP192_PWM1_DUTY_RATIO_Y1;
    write_buf[1] = 0xFF;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 9Ah
    // PWM1 duty cycle Y2
    write_buf[0] = AXP192_PWM1_DUTY_RATIO_Y2;
    write_buf[1] = 0xFF;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 93h
    // Speaker off (GPIO2 output low)
    write_buf[0] = AXP192_GPIO2_FUNCTION;
    write_buf[1] = AXP192_GPIO2_FUNCTION_LOW_OUTPUT;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 28h
    // LDO2 (SD + TFT Logic): 3.3V
    // LDO3 (Vibration motore): Set to minimum voltage 1.8V
    write_buf[0] = AXP192_LDO23_OUT_VOLTAGE;
    write_buf[1] = AXP192_LDO23_OUT_VOLTAGE_LDO2_3_3V |
        AXP192_LDO23_OUT_VOLTAGE_LDO3_1_8V;
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

    // Reg: 27h
    // DCDC3 (TFT backlight): 3.0V
    write_buf[0] = AXP192_DCDC3_OUT_VOLTAGE;
    write_buf[1] = AXP192_DCDC3_OUT_VOLTAGE_3_0V;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 12h
    // Enable:
    // DCDC1    ESP32 VDD, touch screen and 3.3V on M-Bus
    // DCDC3    LCD backlight
    // LDO2     LCD logic and SD card
    write_buf[0] = AXP192_DCDC13_LDO23_CTRL;
    write_buf[1] = AXP192_DCDC13_LDO23_CTRL_LDO2 |
        AXP192_DCDC13_LDO23_CTRL_DCDC3 |
        AXP192_DCDC13_LDO23_CTRL_DCDC1;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 92h
    // Set GPIO1 Sys green LED output as PWM
    // PWM duty cycle is set to 100% therefore sys led is off
    write_buf[0] = AXP192_GPIO1_FUNCTION;
    write_buf[1] = AXP192_GPIO1_FUNCTION_PWM1_OUTPUT;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    /*
    * Select power source:
    * If there is voltage on VBUS means that BUS_5V is powered from M-BUS
    * and it can use VBUS as power source. If no voltage is present on VBUS
    * disable VBUS and rely on ACIN (USB_5V) as power source.
    */
    write_buf[0] = AXP192_INPUT_POWER_STATE;
    rc = common_hal_busio_i2c_write_read(i2c, AXP192_I2C_ADDRESS, write_buf, 1, read_buf, sizeof(read_buf));
    if (rc != 0) {
        return false;
    }
    uint8_t powersource = (read_buf[0] & AXP192_INPUT_POWER_STATE_VBUS_AVAILABLE) ?
        PMIC_POWER_SOURCE_M_BUS : PMIC_POWER_SOURCE_USB;
    if (!pmic_set_power_source(powersource, i2c)) {
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
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        &pin_GPIO15,    // DC
        &pin_GPIO5,     // CS
        NULL,           // RST
        32000000,       // baudrate
        0,              // polarity
        0               // phase
        );

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        320,            // width (after rotation)
        240,            // height (after rotation)
        0,              // column start
        0,              // row start
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
        NULL,           // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f,           // brightness
        false,          // single_byte_bounds
        false,          // data_as_commands
        true,           // auto_refresh
        61,             // native_frames_per_second
        true,           // backlight_on_high
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
