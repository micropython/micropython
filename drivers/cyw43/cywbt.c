/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/mpbthci.h"

#if MICROPY_PY_NETWORK_CYW43

#include "lib/cyw43-driver/src/cyw43_config.h"
#include "lib/cyw43-driver/firmware/cyw43_btfw_4343A1.h"

// Provided by the port, and also possibly shared with the stack.
extern uint8_t mp_bluetooth_hci_cmd_buf[4 + 256];

/******************************************************************************/
// CYW BT HCI low-level driver

#ifdef CYW43_PIN_BT_CTS
// This code is not portable and currently only builds on stm32 port.

#include "pin_static_af.h"
#include "uart.h"

// Provided by the port.
extern machine_uart_obj_t mp_bluetooth_hci_uart_obj;

static void cywbt_wait_cts_low(void) {
    mp_hal_pin_config(CYW43_PIN_BT_CTS, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0);
    for (int i = 0; i < 200; ++i) {
        if (mp_hal_pin_read(CYW43_PIN_BT_CTS) == 0) {
            break;
        }
        mp_hal_delay_ms(1);
    }
    mp_hal_pin_config_alt(CYW43_PIN_BT_CTS, MP_HAL_PIN_MODE_ALT,
        MP_HAL_PIN_PULL_UP, AF_FN_UART, mp_bluetooth_hci_uart_obj.uart_id);
}
#endif

static int cywbt_hci_cmd_raw(size_t len, uint8_t *buf) {
    mp_bluetooth_hci_uart_write((void *)buf, len);
    for (int c, i = 0; i < 6; ++i) {
        while ((c = mp_bluetooth_hci_uart_readchar()) == -1) {
            mp_event_wait_indefinite();
        }
        buf[i] = c;
    }

    // expect a command complete event (event 0x0e)
    if (buf[0] != 0x04 || buf[1] != 0x0e) {
        printf("unknown response: %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3]);
        return -1;
    }

    /*
    if buf[3:6] != cmd[:3]:
        print('response doesn\'t match cmd:', cmd, ev)
        return b''
        */

    int sz = buf[2] - 3;
    for (int c, i = 0; i < sz; ++i) {
        while ((c = mp_bluetooth_hci_uart_readchar()) == -1) {
            mp_event_wait_indefinite();
        }
        buf[i] = c;
    }

    return 0;
}

static int cywbt_hci_cmd(int ogf, int ocf, size_t param_len, const uint8_t *param_buf) {
    uint8_t *buf = mp_bluetooth_hci_cmd_buf;
    buf[0] = 0x01;
    buf[1] = ocf;
    buf[2] = ogf << 2 | ocf >> 8;
    buf[3] = param_len;
    if (param_len) {
        memcpy(buf + 4, param_buf, param_len);
    }
    return cywbt_hci_cmd_raw(4 + param_len, buf);
}

static void put_le16(uint8_t *buf, uint16_t val) {
    buf[0] = val;
    buf[1] = val >> 8;
}

static void put_le32(uint8_t *buf, uint32_t val) {
    buf[0] = val;
    buf[1] = val >> 8;
    buf[2] = val >> 16;
    buf[3] = val >> 24;
}

static int cywbt_set_baudrate(uint32_t baudrate) {
    uint8_t buf[6];
    put_le16(buf, 0);
    put_le32(buf + 2, baudrate);
    return cywbt_hci_cmd(0x3f, 0x18, 6, buf);
}

// download firmware
static int cywbt_download_firmware(const uint8_t *firmware) {
    cywbt_hci_cmd(0x3f, 0x2e, 0, NULL);

    bool last_packet = false;
    while (!last_packet) {
        uint8_t *buf = mp_bluetooth_hci_cmd_buf;
        memcpy(buf + 1, firmware, 3);
        firmware += 3;
        last_packet = buf[1] == 0x4e;
        if (buf[2] != 0xfc) {
            printf("fail1 %02x\n", buf[2]);
            break;
        }
        uint8_t len = buf[3];

        memcpy(buf + 4, firmware, len);
        firmware += len;

        buf[0] = 1;
        cywbt_hci_cmd_raw(4 + len, buf);
        if (buf[0] != 0) {
            printf("fail3 %02x\n", buf[0]);
            break;
        }
    }

    // RF switch must select high path during BT patch boot
    #if MICROPY_HW_ENABLE_RF_SWITCH
    mp_hal_pin_config(CYW43_PIN_WL_GPIO_1, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0);
    #endif
    mp_hal_delay_ms(10); // give some time for CTS to go high
    #ifdef CYW43_PIN_BT_CTS
    cywbt_wait_cts_low();
    #endif
    #if MICROPY_HW_ENABLE_RF_SWITCH
    // Select chip antenna (could also select external)
    mp_hal_pin_config(CYW43_PIN_WL_GPIO_1, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_DOWN, 0);
    #endif

    mp_bluetooth_hci_uart_set_baudrate(115200);
    cywbt_set_baudrate(MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY);
    mp_bluetooth_hci_uart_set_baudrate(MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY);

    return 0;
}

int mp_bluetooth_hci_controller_init(void) {
    // This is called immediately after the UART is initialised during stack initialisation.

    mp_hal_pin_output(CYW43_PIN_BT_REG_ON);
    mp_hal_pin_low(CYW43_PIN_BT_REG_ON);
    #ifdef CYW43_PIN_BT_HOST_WAKE
    mp_hal_pin_input(CYW43_PIN_BT_HOST_WAKE);
    #endif
    #ifdef CYW43_PIN_BT_DEV_WAKE
    mp_hal_pin_output(CYW43_PIN_BT_DEV_WAKE);
    mp_hal_pin_low(CYW43_PIN_BT_DEV_WAKE);
    #endif

    #if MICROPY_HW_ENABLE_RF_SWITCH
    // TODO don't select antenna if wifi is enabled
    mp_hal_pin_config(CYW43_PIN_WL_GPIO_4, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0); // RF-switch power
    mp_hal_pin_high(CYW43_PIN_WL_GPIO_4); // Turn the RF-switch on
    #endif

    uint8_t buf[256];

    mp_hal_pin_low(CYW43_PIN_BT_REG_ON);
    mp_bluetooth_hci_uart_set_baudrate(115200);
    mp_hal_delay_ms(100);
    mp_hal_pin_high(CYW43_PIN_BT_REG_ON);
    #ifdef CYW43_PIN_BT_CTS
    cywbt_wait_cts_low();
    #else
    mp_hal_delay_ms(100);
    #endif

    // Reset
    cywbt_hci_cmd(0x03, 0x0003, 0, NULL);

    #ifdef MICROPY_HW_BLE_UART_BAUDRATE_DOWNLOAD_FIRMWARE
    // Change baudrate
    cywbt_set_baudrate(MICROPY_HW_BLE_UART_BAUDRATE_DOWNLOAD_FIRMWARE);
    mp_bluetooth_hci_uart_set_baudrate(MICROPY_HW_BLE_UART_BAUDRATE_DOWNLOAD_FIRMWARE);
    #endif

    cywbt_download_firmware((const uint8_t *)&cyw43_btfw_4343A1[0]);

    // Reset
    cywbt_hci_cmd(0x03, 0x0003, 0, NULL);

    // Set BD_ADDR (sent as little endian)
    uint8_t bdaddr[6];
    mp_hal_get_mac(MP_HAL_MAC_BDADDR, bdaddr);
    buf[0] = bdaddr[5];
    buf[1] = bdaddr[4];
    buf[2] = bdaddr[3];
    buf[3] = bdaddr[2];
    buf[4] = bdaddr[1];
    buf[5] = bdaddr[0];
    cywbt_hci_cmd(0x3f, 0x0001, 6, buf);

    // Set local name
    // memset(buf, 0, 248);
    // memcpy(buf, "PYBD-BLE", 8);
    // cywbt_hci_cmd(0x03, 0x0013, 248, buf);

    // Configure sleep mode
    cywbt_hci_cmd(0x3f, 0x27, 12, (const uint8_t *)"\x01\x02\x02\x00\x00\x00\x01\x00\x00\x00\x00\x00");

    // HCI_Write_LE_Host_Support
    cywbt_hci_cmd(3, 109, 2, (const uint8_t *)"\x01\x00");

    #ifdef CYW43_PIN_BT_DEV_WAKE
    mp_hal_pin_high(CYW43_PIN_BT_DEV_WAKE); // let sleep
    #endif

    return 0;
}

int mp_bluetooth_hci_controller_deinit(void) {
    mp_hal_pin_low(CYW43_PIN_BT_REG_ON);

    return 0;
}

#ifdef CYW43_PIN_BT_DEV_WAKE
static uint32_t bt_sleep_ticks;
#endif

int mp_bluetooth_hci_controller_sleep_maybe(void) {
    #ifdef CYW43_PIN_BT_DEV_WAKE
    if (mp_hal_pin_read(CYW43_PIN_BT_DEV_WAKE) == 0) {
        if (mp_hal_ticks_ms() - bt_sleep_ticks > 500) {
            mp_hal_pin_high(CYW43_PIN_BT_DEV_WAKE); // let sleep
        }
    }
    #endif
    return 0;
}

bool mp_bluetooth_hci_controller_woken(void) {
    #ifdef CYW43_PIN_BT_HOST_WAKE
    bool host_wake = mp_hal_pin_read(CYW43_PIN_BT_HOST_WAKE);
    /*
    // this is just for info/tracing purposes
    static bool last_host_wake = false;
    if (host_wake != last_host_wake) {
        printf("HOST_WAKE change %d -> %d\n", last_host_wake, host_wake);
        last_host_wake = host_wake;
    }
    */
    return host_wake;
    #else
    return true;
    #endif
}

int mp_bluetooth_hci_controller_wakeup(void) {
    #ifdef CYW43_PIN_BT_DEV_WAKE
    bt_sleep_ticks = mp_hal_ticks_ms();

    if (mp_hal_pin_read(CYW43_PIN_BT_DEV_WAKE) == 1) {
        mp_hal_pin_low(CYW43_PIN_BT_DEV_WAKE); // wake up
        // Use delay_us rather than delay_ms to prevent running the scheduler (which
        // might result in more BLE operations).
        mp_hal_delay_us(5000); // can't go lower than this
    }
    #endif

    return 0;
}

#endif
