/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
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
 *
 * ESP-Hosted Bluetooth HCI driver.
 */

#include "py/mphal.h"

#if MICROPY_PY_BLUETOOTH && MICROPY_PY_NETWORK_ESP_HOSTED

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "extmod/mpbthci.h"
#include "esp_hosted_hal.h"

#define HCI_COMMAND_PACKET      (0x01)
#define HCI_ACLDATA_PACKET      (0x02)
#define HCI_EVENT_PACKET        (0x04)

#define HCI_COMMAND_COMPLETE    (0x0e)
#define HCI_COMMAND_TIMEOUT     (3000)

#define OGF_LINK_CTL            (0x01)
#define OGF_HOST_CTL            (0x03)

#define OCF_SET_EVENT_MASK      (0x0001)
#define OCF_RESET               (0x0003)

// Provided by the port, and also possibly shared with the stack.
extern uint8_t mp_bluetooth_hci_cmd_buf[4 + 256];

int esp_hosted_hci_cmd(int ogf, int ocf, size_t param_len, const uint8_t *param_buf) {
    uint8_t *buf = mp_bluetooth_hci_cmd_buf;

    buf[0] = HCI_COMMAND_PACKET;
    buf[1] = ocf;
    buf[2] = ogf << 2 | ocf >> 8;
    buf[3] = param_len;

    if (param_len) {
        memcpy(buf + 4, param_buf, param_len);
    }

    debug_printf("HCI Command: %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3]);

    mp_bluetooth_hci_uart_write(buf, 4 + param_len);

    // Receive HCI event packet, initially reading 3 bytes (HCI Event, Event code, Plen).
    for (mp_uint_t start = mp_hal_ticks_ms(), size = 3, i = 0; i < size;) {
        while (!mp_bluetooth_hci_uart_any()) {
            mp_event_wait_ms(1);
            // Timeout.
            if ((mp_hal_ticks_ms() - start) > HCI_COMMAND_TIMEOUT) {
                error_printf("timeout waiting for HCI packet\n");
                return -1;
            }
        }

        buf[i] = mp_bluetooth_hci_uart_readchar();

        // There seems to be a sync issue with this fw/module.
        if (i == 0 && buf[0] == 0xFF) {
            continue;
        }

        // Check for packet type.
        if (i == 0 && buf[0] != HCI_EVENT_PACKET) {
            error_printf("unexpected HCI packet: %02x\n", buf[0]);
            return -1;
        }

        // Sanity check the packet parameters length.
        if (i == 2 && ((size += buf[2]) > sizeof(mp_bluetooth_hci_cmd_buf))) {
            error_printf("unexpected event packet length: %d\n", size);
            return -1;
        }

        i++;
    }

    // We're only looking for command complete events.
    if (buf[1] != HCI_COMMAND_COMPLETE || buf[4] != ocf || buf[5] != (ogf << 2 | ocf >> 8)) {
        error_printf("response mismatch: %02x %02x\n", buf[4], buf[5]);
        return -1;
    }

    // Log event.
    debug_printf("HCI Event packet: %02x %02x %02x %02x %02x %02x %02x\n",
        buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);

    // Status code.
    return buf[6];
}

int mp_bluetooth_hci_controller_init(void) {
    // Low-level pins init, memory pool allocation etc...
    esp_hosted_hal_init(ESP_HOSTED_MODE_BT);

    mp_uint_t start = mp_hal_ticks_ms();
    // Skip bootloader messages.
    while ((mp_hal_ticks_ms() - start) < 2500) {
        if (mp_bluetooth_hci_uart_any()) {
            mp_bluetooth_hci_uart_readchar();
        }
        mp_event_wait_ms(1);
    }

    #ifdef MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY
    mp_bluetooth_hci_uart_set_baudrate(MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY);
    #endif

    mp_hal_pin_output(MICROPY_HW_BLE_UART_RTS);
    mp_hal_pin_write(MICROPY_HW_BLE_UART_RTS, 0);

    // Send reset command
    // It seems that nothing else is needed for now.
    return esp_hosted_hci_cmd(OGF_HOST_CTL, OCF_RESET, 0, NULL);
}

int mp_bluetooth_hci_controller_deinit(void) {
    esp_hosted_hal_deinit();
    return 0;
}

#endif
