/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "pin_static_af.h"
#include "uart.h"
#include "cywbt.h"
#include "nimble/hci_uart.h"

#if MICROPY_PY_NETWORK_CYW43

extern const char fw_4343WA1_7_45_98_50_start;
#define CYWBT_FW_ADDR (&fw_4343WA1_7_45_98_50_start + 749 * 512 + 29 * 256)

/******************************************************************************/
// CYW BT HCI low-level driver

static void cywbt_wait_cts_low(void) {
    mp_hal_pin_config(pyb_pin_BT_CTS, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0);
    for (int i = 0; i < 200; ++i) {
        if (mp_hal_pin_read(pyb_pin_BT_CTS) == 0) {
            break;
        }
        mp_hal_delay_ms(1);
    }
    mp_hal_pin_config_alt_static(pyb_pin_BT_CTS, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_USART6_CTS);
}

static int cywbt_hci_cmd_raw(size_t len, uint8_t *buf) {
    uart_tx_strn(&bt_hci_uart_obj, (void*)buf, len);
    for (int i = 0; i < 6; ++i) {
        while (!uart_rx_any(&bt_hci_uart_obj)) {
            MICROPY_EVENT_POLL_HOOK
        }
        buf[i] = uart_rx_char(&bt_hci_uart_obj);
    }

    // expect a comand complete event (event 0x0e)
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
    for (int i = 0; i < sz; ++i) {
        while (!uart_rx_any(&bt_hci_uart_obj)) {
            MICROPY_EVENT_POLL_HOOK
        }
        buf[i] = uart_rx_char(&bt_hci_uart_obj);
    }

    return 0;
}

static int cywbt_hci_cmd(int ogf, int ocf, size_t param_len, const uint8_t *param_buf) {
    uint8_t *buf = bt_hci_cmd_buf;
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
        uint8_t *buf = bt_hci_cmd_buf;
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
    mp_hal_pin_config(pyb_pin_WL_GPIO_1, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0);
    mp_hal_delay_ms(10); // give some time for CTS to go high
    cywbt_wait_cts_low();
    mp_hal_pin_config(pyb_pin_WL_GPIO_1, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_DOWN, 0); // Select chip antenna (could also select external)

    nimble_hci_uart_set_baudrate(115200);
    cywbt_set_baudrate(3000000);
    nimble_hci_uart_set_baudrate(3000000);

    return 0;
}

int cywbt_init(void) {
    // This is called from Nimble via hal_uart_config which will have already initialized the UART.

    mp_hal_pin_output(pyb_pin_BT_REG_ON);
    mp_hal_pin_low(pyb_pin_BT_REG_ON);
    mp_hal_pin_input(pyb_pin_BT_HOST_WAKE);
    mp_hal_pin_output(pyb_pin_BT_DEV_WAKE);
    mp_hal_pin_low(pyb_pin_BT_DEV_WAKE);

    // TODO don't select antenna if wifi is enabled
    mp_hal_pin_config(pyb_pin_WL_GPIO_4, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0); // RF-switch power
    mp_hal_pin_high(pyb_pin_WL_GPIO_4); // Turn the RF-switch on

    return 0;
}

int cywbt_activate(void) {
    uint8_t buf[256];

    mp_hal_pin_low(pyb_pin_BT_REG_ON);
    nimble_hci_uart_set_baudrate(115200);
    mp_hal_delay_ms(100);
    mp_hal_pin_high(pyb_pin_BT_REG_ON);
    cywbt_wait_cts_low();

    // Reset
    cywbt_hci_cmd(0x03, 0x0003, 0, NULL);

    // Change baudrate
    cywbt_set_baudrate(3000000);
    nimble_hci_uart_set_baudrate(3000000);

    cywbt_download_firmware((const uint8_t*)CYWBT_FW_ADDR);

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
    cywbt_hci_cmd(0x3f, 0x27, 12, (const uint8_t*)"\x01\x02\x02\x00\x00\x00\x01\x00\x00\x00\x00\x00");

    // HCI_Write_LE_Host_Support
    cywbt_hci_cmd(3, 109, 2, (const uint8_t*)"\x01\x00");

    mp_hal_pin_high(pyb_pin_BT_DEV_WAKE); // let sleep

    return 0;
}

#endif
