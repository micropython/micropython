/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jim Mussared
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

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_BLUETOOTH && (MICROPY_BLUETOOTH_NIMBLE || (MICROPY_BLUETOOTH_BTSTACK && MICROPY_BLUETOOTH_BTSTACK_H4))

#if !MICROPY_PY_THREAD
#error Unix HCI UART requires MICROPY_PY_THREAD
#endif

#if !MICROPY_PY_THREAD_RTOS
#error Unix HCI UART requires MICROPY_PY_THREAD_RTOS
#endif

#include "extmod/modbluetooth.h"
#include "extmod/mpbthci.h"

#include <pthread.h>
#include <unistd.h>

#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>

#define DEBUG_printf(...) // printf(__VA_ARGS__)

#define HCI_TRACE (0)
#define COL_OFF "\033[0m"
#define COL_GREEN "\033[0;32m"
#define COL_BLUE "\033[0;34m"

uint8_t mp_bluetooth_hci_cmd_buf[4 + 256];

STATIC int uart_fd = -1;

// Must be provided by the stack bindings (e.g. mpnimbleport.c or mpbtstackport.c).
extern bool mp_bluetooth_run_hci_uart(void);
extern bool mp_bluetooth_run_host_stack(void);

// Allows the stack to tell us that we should stop running the hci poll loop.
extern bool mp_bluetooth_hci_active(void);

STATIC const useconds_t UART_POLL_INTERVAL_MS = 200;
STATIC const useconds_t TIMER_POLL_INTERVAL_MS = 1;
STATIC pthread_t hci_uart_poll_thread_id;
STATIC pthread_t hci_event_poll_thread_id;

// This simulates a UART RX irq that delivers incoming UART data into the host
// stack as it arrives.
STATIC void *hci_uart_poll_thread(void *arg) {
    (void)arg;

    DEBUG_printf("hci_uart_poll_thread: start\n");

    while (mp_bluetooth_hci_active()) {
        // Wait for the UART to become readable (or closed).
        struct pollfd fd = { .fd = uart_fd, .events = POLLIN | POLLHUP };
        int ret = poll(&fd, 1, UART_POLL_INTERVAL_MS);
        if (ret > 0 && (fd.revents & POLLIN)) {
            mp_bluetooth_run_hci_uart();
        }
    }

    DEBUG_printf("~hci_uart_poll_thread\n");

    return NULL;
}

// We run the host stack periodically (every 1ms) by a background thread.
// Because MICROPY_PY_THREAD_RTOS is enabled, when the host stack tries
// to call into Python, it will use mp_thread_run_on_mp_thread to run
// the callbacks on a Python thread.
STATIC void *hci_event_poll_thread(void *arg) {
    (void)arg;

    DEBUG_printf("hci_event_poll_thread: start\n");

    while (mp_bluetooth_hci_active()) {
        // TODO: A potential optimisation here would be to immediately wake up
        // hci_event_poll_thread when the UART thread receives data, which
        // would decrease the latency of responding to the incoming UART data,
        // and allow a longer (more efficient) sleep interval. Alternatively,
        // this could know more about the stack's internals (e.g. length of
        // queue or next timer event).
        usleep(TIMER_POLL_INTERVAL_MS * 1000);
        mp_bluetooth_run_host_stack();
    }

    DEBUG_printf("~hci_event_poll_thread\n");

    return NULL;
}

STATIC int configure_uart(void) {
    struct termios toptions;

    // Get existing config.
    if (tcgetattr(uart_fd, &toptions) < 0) {
        DEBUG_printf("Couldn't get term attributes");
        return -1;
    }

    // Raw mode (disable all processing).
    cfmakeraw(&toptions);

    // 8N1, no parity.
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag |= CS8;
    toptions.c_cflag &= ~PARENB;

    // Enable receiver, ignore modem control lines
    toptions.c_cflag |= CREAD | CLOCAL;

    // Blocking, single-byte reads.
    toptions.c_cc[VMIN] = 1;
    toptions.c_cc[VTIME] = 0;

    // Enable HW RTS/CTS flow control.
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY);
    toptions.c_cflag |= CRTSCTS;

    // 1Mbit (TODO: make this configurable).
    speed_t brate = B1000000;
    cfsetospeed(&toptions, brate);
    cfsetispeed(&toptions, brate);

    // Apply immediately.
    if (tcsetattr(uart_fd, TCSANOW, &toptions) < 0) {
        DEBUG_printf("Couldn't set term attributes");

        close(uart_fd);
        uart_fd = -1;

        return -1;
    }

    return 0;
}

pthread_mutex_t nimble_mutex;

// HCI UART bindings.
int mp_bluetooth_hci_uart_init(uint32_t port, uint32_t baudrate) {
    (void)port;
    (void)baudrate;

    DEBUG_printf("mp_bluetooth_hci_uart_init (unix)\n");

    if (uart_fd != -1) {
        DEBUG_printf("mp_bluetooth_hci_uart_init: already active\n");
        return 0;
    }

    char uart_device_name[256] = "/dev/ttyUSB0";

    char *path = getenv("MICROPYBTUART");
    if (path != NULL) {
        strcpy(uart_device_name, path);
    }
    DEBUG_printf("mp_bluetooth_hci_uart_init: Using HCI UART: %s\n", uart_device_name);

    int flags = O_RDWR | O_NOCTTY | O_NONBLOCK;
    uart_fd = open(uart_device_name, flags);
    if (uart_fd == -1) {
        printf("mp_bluetooth_hci_uart_init: Unable to open port %s\n", uart_device_name);
        return -1;
    }

    if (configure_uart()) {
        return -1;
    }

    pthread_mutexattr_t nimble_mutex_attr;
    pthread_mutexattr_init(&nimble_mutex_attr);
    pthread_mutexattr_settype(&nimble_mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&nimble_mutex, &nimble_mutex_attr);

    // Create a threads to run the uart and the host stack.
    pthread_attr_t joinable_attr;
    pthread_attr_init(&joinable_attr);
    pthread_attr_setdetachstate(&joinable_attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&hci_uart_poll_thread_id, &joinable_attr, &hci_uart_poll_thread, NULL);
    pthread_create(&hci_event_poll_thread_id, &joinable_attr, &hci_event_poll_thread, NULL);

    return 0;
}

int mp_bluetooth_hci_uart_deinit(void) {
    DEBUG_printf("mp_bluetooth_hci_uart_deinit\n");

    if (uart_fd == -1) {
        DEBUG_printf("--> nothing to do\n");
        return 0;
    }

    // Close the UART.
    close(uart_fd);
    uart_fd = -1;

    // Wait for the uart + host stack threads to terminate when the state is set to OFF.
    pthread_join(hci_uart_poll_thread_id, NULL);
    pthread_join(hci_event_poll_thread_id, NULL);

    DEBUG_printf("~mp_bluetooth_hci_uart_deinit\n");

    return 0;
}

int mp_bluetooth_hci_uart_set_baudrate(uint32_t baudrate) {
    (void)baudrate;
    DEBUG_printf("mp_bluetooth_hci_uart_set_baudrate\n");
    return 0;
}

int mp_bluetooth_hci_uart_readchar(void) {
    // DEBUG_printf("mp_bluetooth_hci_uart_readchar\n");

    if (uart_fd == -1) {
        return -1;
    }

    uint8_t c;
    ssize_t bytes_read = read(uart_fd, &c, 1);

    if (bytes_read == 1) {
        #if HCI_TRACE
        printf(COL_BLUE "> [% 8ld] RX: %02x" COL_OFF "\n", mp_hal_ticks_ms(), c);
        #endif
        return c;
    } else {
        return -1;
    }
}

int mp_bluetooth_hci_uart_write(const uint8_t *buf, size_t len) {
    // DEBUG_printf("mp_bluetooth_hci_uart_write\n");

    if (uart_fd == -1) {
        return 0;
    }

    #if HCI_TRACE
    printf(COL_GREEN "< [% 8ld] TX: %02x", mp_hal_ticks_ms(), buf[0]);
    for (size_t i = 1; i < len; ++i) {
        printf(":%02x", buf[i]);
    }
    printf(COL_OFF "\n");
    #endif

    return write(uart_fd, buf, len);
}

// No-op implementations of HCI controller interface.
int mp_bluetooth_hci_controller_init(void) {
    return 0;
}

int mp_bluetooth_hci_controller_deinit(void) {
    return 0;
}

int mp_bluetooth_hci_controller_sleep_maybe(void) {
    return 0;
}

bool mp_bluetooth_hci_controller_woken(void) {
    return true;
}

int mp_bluetooth_hci_controller_wakeup(void) {
    return 0;
}

#endif // MICROPY_PY_BLUETOOTH && (MICROPY_BLUETOOTH_NIMBLE || (MICROPY_BLUETOOTH_BTSTACK && MICROPY_BLUETOOTH_BTSTACK_H4))
