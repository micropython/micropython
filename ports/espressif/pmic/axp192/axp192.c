// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "axp192.h"

bool pmic_common_init(busio_i2c_obj_t *i2c) {
    int rc;
    uint8_t write_buf[2];

    // Reg: 31h
    // VOFF Shutdown voltage setting ( 3.0V )
    write_buf[0] = AXP192_POWER_OFF_VOLTAGE;
    write_buf[1] = AXP192_POWER_OFF_VOLTAGE_3_0V;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 32h
    // Enable battery monitoring
    // N_OE shout down delay 2 seconds
    write_buf[0] = AXP192_POWER_OFF_BATT_CHGLED_CTRL;
    write_buf[1] = AXP192_POWER_OFF_BATT_CHGLED_CTRL_BATT_MONITOR_ON |
        AXP192_POWER_OFF_BATT_CHGLED_CTRL_N_OE_SHUTDOWN_DELAY_2_0S;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 35h
    // Enable RTC battery charge: 3.0V, 200uA
    write_buf[0] = AXP192_BACKUP_BATT;
    write_buf[1] = AXP192_BACKUP_BATT_CHARGING_ENABLE |
        AXP192_BACKUP_BATT_CHARGING_VOLTAGE_3_0V |
        AXP192_BACKUP_BATT_CHARGING_CURRENT_200uA;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 36h
    // Power on:  Short press 128ms
    // Power off: Long press 1s
    // Power OK delay 64ms
    // Power off delay 4s
    write_buf[0] = AXP192_PEK;
    write_buf[1] = AXP192_PEK_SHORT_PRESS_128mS |
        AXP192_PEK_LONG_PRESS_1_0S |
        AXP192_PEK_LONG_PRESS_POWER_OFF |
        AXP192_PEK_PWROK_DELAY_64mS |
        AXP192_PEK_POWER_OFF_TIME_4S;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 3Ah
    // APS Low battery warning level 1: 3.695V
    write_buf[0] = AXP192_APS_LOW_BATT_LEVEL_1;
    write_buf[1] = AXP192_APS_LOW_BATT_VOLTAGE_3_695V;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 3Bh
    // APS Low battery warning level 2: 3.600V
    write_buf[0] = AXP192_APS_LOW_BATT_LEVEL_2;
    write_buf[1] = AXP192_APS_LOW_BATT_VOLTAGE_3_600V;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 82h
    // ADC all on
    write_buf[0] = AXP192_ADC_ENABLE_1;
    write_buf[1] = AXP192_ADC_ENABLE_1_BATT_VOL |
        AXP192_ADC_ENABLE_1_BATT_CUR |
        AXP192_ADC_ENABLE_1_ACIN_VOL |
        AXP192_ADC_ENABLE_1_ACIN_CUR |
        AXP192_ADC_ENABLE_1_VBUS_VOL |
        AXP192_ADC_ENABLE_1_VBUS_CUR |
        AXP192_ADC_ENABLE_1_APS_VOL |
        AXP192_ADC_ENABLE_1_TS_PIN;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 83h
    // ADC temperature on
    write_buf[0] = AXP192_ADC_ENABLE_2;
    write_buf[1] = AXP192_ADC_ENABLE_2_TEMP_MON;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 84h
    // ADC 25Hz
    write_buf[0] = AXP192_ADC_TS;
    write_buf[1] = AXP192_ADC_TS_SAMPLE_25HZ |
        AXP192_ADC_TS_OUT_CUR_80uA |
        AXP192_ADC_TS_PIN_TEMP_MON |
        AXP192_ADC_TS_PIN_OUT_SAVE_ENG;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    return true;
}

bool pmic_disable_all_irq(busio_i2c_obj_t *i2c) {
    int rc;
    uint8_t write_buf[2];

    // Reg: 40h
    // IRQ enable control register 1
    write_buf[0] = AXP192_IRQ_1_ENABLE;
    write_buf[1] = AXP192_IRQ_X_DISABLE_ALL;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 41h
    // IRQ enable control register 2
    write_buf[0] = AXP192_IRQ_2_ENABLE;
    write_buf[1] = AXP192_IRQ_X_DISABLE_ALL;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 42h
    // IRQ enable control register 3
    // Enable power on key short and long press interrupt
    write_buf[0] = AXP192_IRQ_3_ENABLE;
    write_buf[1] = AXP192_IRQ_X_DISABLE_ALL;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 43h
    // IRQ enable control register 4
    // Enable power on key short and long press interrupt
    write_buf[0] = AXP192_IRQ_4_ENABLE;
    write_buf[1] = AXP192_IRQ_X_DISABLE_ALL;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 4Ah
    // IRQ enable control register 5
    write_buf[0] = AXP192_IRQ_5_ENABLE;
    write_buf[1] = AXP192_IRQ_X_DISABLE_ALL;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    return true;
}

bool pmic_clear_all_irq(busio_i2c_obj_t *i2c) {
    int rc;
    uint8_t write_buf[2];

    // Reg: 44h
    // IRQ enable control register 1
    write_buf[0] = AXP192_IRQ_1_STATUS;
    write_buf[1] = AXP192_IRQ_X_CLEAR_STATUS;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 45h
    // IRQ enable control register 2
    write_buf[0] = AXP192_IRQ_2_STATUS;
    write_buf[1] = AXP192_IRQ_X_CLEAR_STATUS;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 46h
    // IRQ enable control register 3
    write_buf[0] = AXP192_IRQ_3_STATUS;
    write_buf[1] = AXP192_IRQ_X_CLEAR_STATUS;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 47h
    // IRQ enable control register 4
    write_buf[0] = AXP192_IRQ_4_STATUS;
    write_buf[1] = AXP192_IRQ_X_CLEAR_STATUS;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    // Reg: 4Ah
    // IRQ enable control register 5
    write_buf[0] = AXP192_IRQ_5_STATUS;
    write_buf[1] = AXP192_IRQ_X_CLEAR_STATUS;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    return true;
}

bool pmic_enable_power_key_press_irq(busio_i2c_obj_t *i2c) {
    int rc;
    uint8_t read_buf[1];
    uint8_t write_buf[2];

    // Reg: 42h
    // IRQ enable control register 3
    // Enable power on key short and long press interrupt
    write_buf[0] = AXP192_IRQ_2_ENABLE;
    rc = common_hal_busio_i2c_write_read(i2c, AXP192_I2C_ADDRESS, write_buf, 1, read_buf, sizeof(read_buf));
    if (rc != 0) {
        return false;
    }
    write_buf[1] = read_buf[0] | AXP192_IRQ_3_PEK_SHORT_PRESS | AXP192_IRQ_3_PEK_LONG_PRESS;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    return true;
}

bool pmic_enable_low_battery_irq(busio_i2c_obj_t *i2c) {
    int rc;
    uint8_t write_buf[2];

    // Reg: 43h
    // IRQ enable control register 4
    // Enable power on key short and long press interrupt
    write_buf[0] = AXP192_IRQ_2_ENABLE;
    write_buf[1] = AXP192_IRQ_4_LOW_VOLTAGE_WARNING;
    rc = common_hal_busio_i2c_write(i2c, AXP192_I2C_ADDRESS, write_buf, sizeof(write_buf));
    if (rc != 0) {
        return false;
    }

    return true;
}
