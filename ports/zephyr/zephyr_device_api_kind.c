/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MASSDRIVER EI (massdriver.space)
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

#if defined(CONFIG_MICROPY_DYNAMIC_DEVICE_INFOS) || defined(CONFIG_MICROPY_DYNAMIC_PINCTRL)

#include <zephyr/version.h>
#include "zephyr_device.h"

static const char *zephyr_api_kind_str[API_MAX] = {
    [API_ADC] = "adc",
    [API_AUXDISPLAY] = "auxdisplay",
    [API_BBRAM] = "bbram",
    [API_BIOMETRIC] = "biometric",
    [API_BT_HCI] = "bt_hci",
    [API_BUZZER] = "buzzer",
    [API_CACHE] = "cache",
    [API_CAN] = "can",
    [API_CELLULAR] = "cellular",
    [API_CHARGER] = "charger",
    [API_CLOCK_CONTROL] = "clock_control",
    [API_CLOCK_MONITOR] = "clock_monitor",
    [API_COMPARATOR] = "comparator",
    [API_COREDUMP] = "coredump",
    [API_COUNTER] = "counter",
    [API_CRC] = "crc",
    [API_DAC] = "dac",
    [API_DAI] = "dai",
    [API_DALI] = "dali",
    [API_DISK] = "disk",
    [API_DISPLAY] = "display",
    [API_DMA] = "dma",
    [API_EDAC] = "edac",
    [API_EEPROM] = "eeprom",
    [API_EMUL_BBRAM] = "emul_bbram",
    [API_FUEL_GAUGE_EMUL] = "fuel_gauge_emul",
    [API_EMUL] = "emul",
    [API_EMUL_SENSOR] = "emul_sensor",
    [API_EMUL_STUB_DEVICE] = "emul_stub_device",
    [API_ENTROPY] = "entropy",
    [API_EMUL_ESPI] = "emul_espi",
    [API_ESPI] = "espi",
    [API_ESPI_SAF] = "espi_saf",
    [API_FLASH] = "flash",
    [API_FPGA] = "fpga",
    [API_FUEL_GAUGE] = "fuel_gauge",
    [API_GNSS] = "gnss",
    [API_GPIO] = "gpio",
    [API_HAPTICS] = "haptics",
    [API_HWINFO] = "hwinfo",
    [API_HWSPINLOCK] = "hwspinlock",
    [API_I2C_EMUL] = "i2c_emul",
    [API_I2C] = "i2c",
    [API_I2S] = "i2s",
    [API_I3C] = "i3c",
    [API_IPM] = "ipm",
    [API_LED] = "led",
    [API_LED_STRIP] = "led_strip",
    [API_LOOPBACK_DISK] = "loopback_disk",
    [API_LORA] = "lora",
    [API_MBOX] = "mbox",
    [API_MDIO] = "mdio",
    [API_MEMC] = "memc",
    [API_MIPI_DBI] = "mipi_dbi",
    [API_MIPI_DSI] = "mipi_dsi",
    [API_EMUL_MSPI] = "emul_mspi",
    [API_MSPI] = "mspi",
    [API_MUX_CONTROL] = "mux_control",
    [API_OPAMP] = "opamp",
    [API_OTP] = "otp",
    [API_PECI] = "peci",
    [API_PINCTRL] = "pinctrl",
    [API_PM_CPU_OPS] = "pm_cpu_ops",
    [API_PS2] = "ps2",
    [API_PTP_CLOCK] = "ptp_clock",
    [API_PULSE_IO] = "pulse_io",
    [API_PWM] = "pwm",
    [API_REGULATOR] = "regulator",
    [API_RESET] = "reset",
    [API_RETAINED_MEM] = "retained_mem",
    [API_RTC] = "rtc",
    [API_SDHC] = "sdhc",
    [API_SENSOR_ATTRIBUTE_TYPES] = "sensor_attribute_types",
    [API_SENSOR_CLOCK] = "sensor_clock",
    [API_SENSOR_DATA_TYPES] = "sensor_data_types",
    [API_SENSOR] = "sensor",
    [API_SMBUS] = "smbus",
    [API_SPI_EMUL] = "spi_emul",
    [API_SPI] = "spi",
    [API_SWDP] = "swdp",
    [API_SYSCON] = "syscon",
    [API_TEE] = "tee",
    [API_TGPIO] = "tgpio",
    [API_UAOL] = "uaol",
    [API_UART_EMUL] = "uart_emul",
    [API_UART] = "uart",
    [API_UART_PIPE] = "uart_pipe",
    [API_VIDEO] = "video",
    [API_VIRTIO] = "virtio",
    [API_W1] = "w1",
    [API_WDT] = "wdt",
    [API_WUC] = "wuc",
};

#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/auxdisplay.h>
#include <zephyr/drivers/bbram.h>
#include <zephyr/drivers/biometrics.h>
#ifdef CONFIG_BT
#include <zephyr/drivers/bluetooth.h>
#endif
#if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
#include <zephyr/drivers/buzzer.h>
#endif
#include <zephyr/drivers/cache.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/cellular.h>
#include <zephyr/drivers/charger.h>
#include <zephyr/drivers/clock_control.h>
#if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
#include <zephyr/drivers/clock_monitor.h>
#endif
#include <zephyr/drivers/comparator.h>
#include <zephyr/drivers/coredump.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/drivers/crc.h>
#include <zephyr/drivers/dac.h>
#include <zephyr/drivers/dai.h>
#if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
#include <zephyr/drivers/dali.h>
#endif
#include <zephyr/drivers/disk.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/dma.h>
#include <zephyr/drivers/edac.h>
#include <zephyr/drivers/eeprom.h>
#include <zephyr/drivers/emul_bbram.h>
#include <zephyr/drivers/emul_fuel_gauge.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/emul_sensor.h>
#include <zephyr/drivers/emul_stub_device.h>
#include <zephyr/drivers/entropy.h>
#if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
#include <zephyr/drivers/espi_emul.h>
#endif
#include <zephyr/drivers/espi.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/drivers/fpga.h>
#include <zephyr/drivers/fuel_gauge.h>
#include <zephyr/drivers/gnss.h>
#include <zephyr/drivers/gpio.h>
#if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
#include <zephyr/drivers/haptics.h>
#endif
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/drivers/hwspinlock.h>
#include <zephyr/drivers/i2c_emul.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/drivers/i3c.h>
#include <zephyr/drivers/ipm.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/drivers/loopback_disk.h>
#include <zephyr/drivers/lora.h>
#include <zephyr/drivers/mbox.h>
#include <zephyr/drivers/mdio.h>
#if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
#include <zephyr/drivers/memc.h>
#endif
#include <zephyr/drivers/mipi_dbi.h>
#include <zephyr/drivers/mipi_dsi.h>
#if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
#include <zephyr/drivers/mspi_emul.h>
#endif
#include <zephyr/drivers/mspi.h>
#if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
#include <zephyr/drivers/mux.h>
#endif
#include <zephyr/drivers/opamp.h>
#include <zephyr/drivers/otp.h>
#include <zephyr/drivers/peci.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/pm_cpu_ops.h>
#include <zephyr/drivers/ps2.h>
#include <zephyr/drivers/ptp_clock.h>
#if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
#include <zephyr/drivers/pulse_io.h>
#endif
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/drivers/reset.h>
#include <zephyr/drivers/retained_mem.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/drivers/sdhc.h>
#include <zephyr/drivers/sensor_attribute_types.h>
#include <zephyr/drivers/sensor_clock.h>
#include <zephyr/drivers/sensor_data_types.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/smbus.h>
#include <zephyr/drivers/spi_emul.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/swdp.h>
#include <zephyr/drivers/syscon.h>
#include <zephyr/drivers/tee.h>
#if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
#include <zephyr/drivers/timeaware_gpio.h>
#endif
#include <zephyr/drivers/uaol.h>
#include <zephyr/drivers/uart_emul.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/uart_pipe.h>
#include <zephyr/drivers/video.h>
#include <zephyr/drivers/virtio.h>
#include <zephyr/drivers/w1.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/drivers/wuc.h>

/* Must be done at runtime and there is genuinely no way around this */
enum zephyr_api_kind device_get_api_kind(const struct device *dev) {
    if (DEVICE_API_IS(adc, dev)) {
        return API_ADC;
    }
    if (DEVICE_API_IS(auxdisplay, dev)) {
        return API_AUXDISPLAY;
    }
    if (DEVICE_API_IS(bbram, dev)) {
        return API_BBRAM;
    }
    if (DEVICE_API_IS(biometric, dev)) {
        return API_BIOMETRIC;
    }
    #ifdef CONFIG_BT
    if (DEVICE_API_IS(bt_hci, dev)) {
        return API_BT_HCI;
    }
    #endif
    #if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
    if (DEVICE_API_IS(buzzer, dev)) {
        return API_BUZZER;
    }
    #endif
    if (DEVICE_API_IS(can, dev)) {
        return API_CAN;
    }
    if (DEVICE_API_IS(cellular, dev)) {
        return API_CELLULAR;
    }
    if (DEVICE_API_IS(charger, dev)) {
        return API_CHARGER;
    }
    if (DEVICE_API_IS(clock_control, dev)) {
        return API_CLOCK_CONTROL;
    }
    #if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
    if (DEVICE_API_IS(clock_monitor, dev)) {
        return API_CLOCK_MONITOR;
    }
    #endif
    if (DEVICE_API_IS(comparator, dev)) {
        return API_COMPARATOR;
    }
    if (DEVICE_API_IS(coredump, dev)) {
        return API_COREDUMP;
    }
    if (DEVICE_API_IS(counter, dev)) {
        return API_COUNTER;
    }
    if (DEVICE_API_IS(crc, dev)) {
        return API_CRC;
    }
    if (DEVICE_API_IS(dac, dev)) {
        return API_DAC;
    }
    if (DEVICE_API_IS(dai, dev)) {
        return API_DAI;
    }
    #if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
    if (DEVICE_API_IS(dali, dev)) {
        return API_DALI;
    }
    #endif
    if (DEVICE_API_IS(display, dev)) {
        return API_DISPLAY;
    }
    if (DEVICE_API_IS(dma, dev)) {
        return API_DMA;
    }
    if (DEVICE_API_IS(edac, dev)) {
        return API_EDAC;
    }
    if (DEVICE_API_IS(eeprom, dev)) {
        return API_EEPROM;
    }
    if (DEVICE_API_IS(emul_bbram, dev)) {
        return API_EMUL_BBRAM;
    }
    if (DEVICE_API_IS(fuel_gauge_emul, dev)) {
        return API_FUEL_GAUGE_EMUL;
    }
    if (DEVICE_API_IS(emul_sensor, dev)) {
        return API_EMUL_SENSOR;
    }
    if (DEVICE_API_IS(entropy, dev)) {
        return API_ENTROPY;
    }
    #if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
    if (DEVICE_API_IS(emul_espi, dev)) {
        return API_EMUL_ESPI;
    }
    #endif
    if (DEVICE_API_IS(espi, dev)) {
        return API_ESPI;
    }
    if (DEVICE_API_IS(flash, dev)) {
        return API_FLASH;
    }
    if (DEVICE_API_IS(fpga, dev)) {
        return API_FPGA;
    }
    if (DEVICE_API_IS(fuel_gauge, dev)) {
        return API_FUEL_GAUGE;
    }
    if (DEVICE_API_IS(gnss, dev)) {
        return API_GNSS;
    }
    if (DEVICE_API_IS(gpio, dev)) {
        return API_GPIO;
    }
    #if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
    if (DEVICE_API_IS(haptics, dev)) {
        return API_HAPTICS;
    }
    #endif
    if (DEVICE_API_IS(hwspinlock, dev)) {
        return API_HWSPINLOCK;
    }
    if (DEVICE_API_IS(i2c, dev)) {
        return API_I2C;
    }
    if (DEVICE_API_IS(i2s, dev)) {
        return API_I2S;
    }
    if (DEVICE_API_IS(i3c, dev)) {
        return API_I3C;
    }
    if (DEVICE_API_IS(ipm, dev)) {
        return API_IPM;
    }
    if (DEVICE_API_IS(led, dev)) {
        return API_LED;
    }
    if (DEVICE_API_IS(led_strip, dev)) {
        return API_LED_STRIP;
    }
    if (DEVICE_API_IS(lora, dev)) {
        return API_LORA;
    }
    if (DEVICE_API_IS(mbox, dev)) {
        return API_MBOX;
    }
    if (DEVICE_API_IS(mdio, dev)) {
        return API_MDIO;
    }
    #if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
    if (DEVICE_API_IS(memc, dev)) {
        return API_MEMC;
    }
    #endif
    if (DEVICE_API_IS(mipi_dbi, dev)) {
        return API_MIPI_DBI;
    }
    if (DEVICE_API_IS(mipi_dsi, dev)) {
        return API_MIPI_DSI;
    }
    #if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
    if (DEVICE_API_IS(emul_mspi, dev)) {
        return API_EMUL_MSPI;
    }
    #endif
    if (DEVICE_API_IS(mspi, dev)) {
        return API_MSPI;
    }
    #if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
    if (DEVICE_API_IS(mux_control, dev)) {
        return API_MUX_CONTROL;
    }
    #endif
    if (DEVICE_API_IS(opamp, dev)) {
        return API_OPAMP;
    }
    if (DEVICE_API_IS(otp, dev)) {
        return API_OTP;
    }
    if (DEVICE_API_IS(peci, dev)) {
        return API_PECI;
    }
    if (DEVICE_API_IS(ps2, dev)) {
        return API_PS2;
    }
    if (DEVICE_API_IS(ptp_clock, dev)) {
        return API_PTP_CLOCK;
    }
    #if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
    if (DEVICE_API_IS(pulse_io, dev)) {
        return API_PULSE_IO;
    }
    #endif
    if (DEVICE_API_IS(pwm, dev)) {
        return API_PWM;
    }
    if (DEVICE_API_IS(regulator, dev)) {
        return API_REGULATOR;
    }
    if (DEVICE_API_IS(reset, dev)) {
        return API_RESET;
    }
    if (DEVICE_API_IS(retained_mem, dev)) {
        return API_RETAINED_MEM;
    }
    if (DEVICE_API_IS(rtc, dev)) {
        return API_RTC;
    }
    if (DEVICE_API_IS(sdhc, dev)) {
        return API_SDHC;
    }
    if (DEVICE_API_IS(sensor, dev)) {
        return API_SENSOR;
    }
    if (DEVICE_API_IS(smbus, dev)) {
        return API_SMBUS;
    }
    if (DEVICE_API_IS(spi, dev)) {
        return API_SPI;
    }
    if (DEVICE_API_IS(syscon, dev)) {
        return API_SYSCON;
    }
    if (DEVICE_API_IS(tee, dev)) {
        return API_TEE;
    }
    #if KERNEL_VERSION_NUMBER >= ZEPHYR_VERSION(4, 4, 99)
    if (DEVICE_API_IS(tgpio, dev)) {
        return API_TGPIO;
    }
    #endif
    if (DEVICE_API_IS(uaol, dev)) {
        return API_UAOL;
    }
    if (DEVICE_API_IS(uart, dev)) {
        return API_UART;
    }
    if (DEVICE_API_IS(video, dev)) {
        return API_VIDEO;
    }
    if (DEVICE_API_IS(virtio, dev)) {
        return API_VIRTIO;
    }
    if (DEVICE_API_IS(w1, dev)) {
        return API_W1;
    }
    if (DEVICE_API_IS(wdt, dev)) {
        return API_WDT;
    }
    if (DEVICE_API_IS(wuc, dev)) {
        return API_WUC;
    }
    return -1;
}

const char *device_get_api_str(enum zephyr_api_kind kind) {
    if (kind >= 0 && kind < ARRAY_SIZE(zephyr_api_kind_str)) {
        return zephyr_api_kind_str[kind];
    }
    return NULL;
}

#endif /* defined(CONFIG_MICROPY_DYNAMIC_DEVICE_INFOS) || defined(CONFIG_MICROPY_DYNAMIC_PINCTRL) */
