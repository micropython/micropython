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

#ifndef MICROPY_INCLUDED_EXTMOD_MPBTHCI_H
#define MICROPY_INCLUDED_EXTMOD_MPBTHCI_H

// --- Optionally can be implemented by the driver. ---------------------------

// Start/stop the HCI controller.
// Requires the UART to this HCI controller is available.
int mp_bluetooth_hci_controller_init(void);
int mp_bluetooth_hci_controller_deinit(void);

// Tell the controller to go to sleep (e.g. on RX if we don't think we're expecting anything more).
int mp_bluetooth_hci_controller_sleep_maybe(void);
// True if the controller woke us up.
bool mp_bluetooth_hci_controller_woken(void);
// Wake up the controller (e.g. we're about to TX).
int mp_bluetooth_hci_controller_wakeup(void);

// --- Bindings that need to be implemented by the port. ----------------------
int mp_bluetooth_hci_uart_init(uint32_t port, uint32_t baudrate);
int mp_bluetooth_hci_uart_deinit(void);
int mp_bluetooth_hci_uart_set_baudrate(uint32_t baudrate);
int mp_bluetooth_hci_uart_any(void);
int mp_bluetooth_hci_uart_readchar(void);
int mp_bluetooth_hci_uart_write(const uint8_t *buf, size_t len);

#endif // MICROPY_INCLUDED_EXTMOD_MPBTHCI_H
