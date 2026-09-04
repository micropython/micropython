/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <zephyr/device.h>
#include "py/obj.h"

const struct device *zephyr_device_find(mp_obj_t ident);
const char *zephyr_device_get_name(const struct device *dev);

#if defined(CONFIG_MICROPY_DYNAMIC_DEVICE_INFOS) || defined(CONFIG_MICROPY_DYNAMIC_PINCTRL)

#if defined(CONFIG_MICROPY_DYNAMIC_PINCTRL)
#include <zephyr/drivers/pinctrl.h>
#endif

// Circumvent limitation of only having devices that are initialized
typedef struct mp_zephyr_device_data_t_ {
    const struct device *dev;
    const char *name;
    const char **labels;
    size_t label_cnt;
    #if defined(CONFIG_MICROPY_DYNAMIC_PINCTRL)
    struct pinctrl_dev_config *pinctrl_cfg;
    pinctrl_soc_pin_t *pinctrl;
    struct pinctrl_state *pinctrl_states;
    #endif
} mp_zephyr_device_data_t;

enum zephyr_api_kind {
    API_ADC = 0,
    API_AUXDISPLAY = 1,
    API_BBRAM = 2,
    API_BIOMETRIC = 3,
    API_BT_HCI = 4,
    API_BUZZER = 5,
    API_CACHE = 6,
    API_CAN = 7,
    API_CELLULAR = 8,
    API_CHARGER = 9,
    API_CLOCK_CONTROL = 10,
    API_CLOCK_MONITOR = 11,
    API_COMPARATOR = 12,
    API_COREDUMP = 13,
    API_COUNTER = 14,
    API_CRC = 15,
    API_DAC = 16,
    API_DAI = 17,
    API_DALI = 18,
    API_DISK = 19,
    API_DISPLAY = 20,
    API_DMA = 21,
    API_EDAC = 22,
    API_EEPROM = 23,
    API_EMUL_BBRAM = 24,
    API_FUEL_GAUGE_EMUL = 25,
    API_EMUL = 26,
    API_EMUL_SENSOR = 27,
    API_EMUL_STUB_DEVICE = 28,
    API_ENTROPY = 29,
    API_EMUL_ESPI = 30,
    API_ESPI = 31,
    API_ESPI_SAF = 32,
    API_FLASH = 33,
    API_FPGA = 34,
    API_FUEL_GAUGE = 35,
    API_GNSS = 36,
    API_GPIO = 37,
    API_HAPTICS = 38,
    API_HWINFO = 39,
    API_HWSPINLOCK = 40,
    API_I2C_EMUL = 41,
    API_I2C = 42,
    API_I2S = 43,
    API_I3C = 44,
    API_IPM = 45,
    API_LED = 46,
    API_LED_STRIP = 47,
    API_LOOPBACK_DISK = 48,
    API_LORA = 49,
    API_MBOX = 50,
    API_MDIO = 51,
    API_MEMC = 52,
    API_MIPI_DBI = 53,
    API_MIPI_DSI = 54,
    API_EMUL_MSPI = 55,
    API_MSPI = 56,
    API_MUX_CONTROL = 57,
    API_OPAMP = 58,
    API_OTP = 59,
    API_PECI = 60,
    API_PINCTRL = 61,
    API_PM_CPU_OPS = 62,
    API_PS2 = 63,
    API_PTP_CLOCK = 64,
    API_PULSE_IO = 65,
    API_PWM = 66,
    API_REGULATOR = 67,
    API_RESET = 68,
    API_RETAINED_MEM = 69,
    API_RTC = 70,
    API_SDHC = 71,
    API_SENSOR_ATTRIBUTE_TYPES = 72,
    API_SENSOR_CLOCK = 73,
    API_SENSOR_DATA_TYPES = 74,
    API_SENSOR = 75,
    API_SMBUS = 76,
    API_SPI_EMUL = 77,
    API_SPI = 78,
    API_SWDP = 79,
    API_SYSCON = 80,
    API_TEE = 81,
    API_TGPIO = 82,
    API_UAOL = 83,
    API_UART_EMUL = 84,
    API_UART = 85,
    API_UART_PIPE = 86,
    API_VIDEO = 87,
    API_VIRTIO = 88,
    API_W1 = 89,
    API_WDT = 90,
    API_WUC = 91,
    API_MAX
};

const mp_zephyr_device_data_t *zephyr_device_find_data_name(mp_obj_t name);
const mp_zephyr_device_data_t *zephyr_device_find_data_dev(const struct device *dev);
const char *device_get_api_str(enum zephyr_api_kind kind);
enum zephyr_api_kind device_get_api_kind(const struct device *dev);

const mp_zephyr_device_data_t *device_find_by_node_label(const char *name);
const mp_zephyr_device_data_t *device_find_by_name(const char *name);
const mp_zephyr_device_data_t *device_find_by_API(const char *api_kind, size_t id);
const mp_zephyr_device_data_t *device_find_by_id(size_t id);

#if defined(CONFIG_MICROPY_DYNAMIC_PINCTRL)
// Supports maximum MPY_MAX_DYNAMIC_PINCTRLS pins, accepts pins and mp_const_none. None are ignored.
int zephyr_device_apply_pinctrl(const mp_zephyr_device_data_t *data, size_t pin_cnt, ...);
#endif

#endif
