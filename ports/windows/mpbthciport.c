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

#include "mpnimbleport.h"
#include "extmod/modbluetooth.h"
#include "extmod/mpbthci.h"

#include <stdio.h>
#include <windows.h>

#include "nimble/hci_common.h"
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2 * !!(condition)]))

#define DEBUG_printf(...)  // printf(__VA_ARGS__)
#define DEBUG_HCI_DUMP (0)

uint8_t mp_bluetooth_hci_cmd_buf[4 + 256];

static HANDLE uart_fd = INVALID_HANDLE_VALUE;

// Must be provided by the stack bindings (e.g. mpnimbleport.c or mpbtstackport.c).
extern bool mp_bluetooth_hci_poll(void);


// These could be moved to MICROPY_PY_THREAD support if/when added.
static CRITICAL_SECTION criticalSection;
static CRITICAL_SECTION *pCriticalSection = NULL;

void mp_thread_windows_init_atomic_section(void) {
    InitializeCriticalSection(&criticalSection);
    pCriticalSection = &criticalSection;
}

void mp_thread_windows_begin_atomic_section(void) {
    if (pCriticalSection != NULL) {
        EnterCriticalSection(pCriticalSection);
    }
}

void mp_thread_windows_end_atomic_section(void) {
    if (pCriticalSection != NULL) {
        LeaveCriticalSection(pCriticalSection);
    }
}


#if MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS

// For synchronous mode, we run all BLE stack code inside a scheduled task.
// This task is scheduled periodically (every 1ms) by a background thread.

// Allows the stack to tell us that we should stop trying to schedule.
extern bool mp_bluetooth_hci_active(void);

// Prevent double-enqueuing of the scheduled task.
static volatile bool events_task_is_scheduled = false;

static mp_obj_t run_events_scheduled_task(mp_obj_t none_in) {
    (void)none_in;
    MICROPY_PY_BLUETOOTH_ENTER
        events_task_is_scheduled = false;
    MICROPY_PY_BLUETOOTH_EXIT
    mp_bluetooth_hci_poll();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(run_events_scheduled_task_obj, run_events_scheduled_task);

#endif // MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS

static const useconds_t UART_POLL_INTERVAL_US = 1000;
static HANDLE hci_poll_thread_id;


static DWORD WINAPI hci_poll_thread(PVOID arg) {

    (void)arg;

    #if MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS
    DEBUG_printf("hci_poll_thread: sync starting\n");

    events_task_is_scheduled = false;

    while (mp_bluetooth_hci_active()) {
        MICROPY_PY_BLUETOOTH_ENTER
        if (!events_task_is_scheduled) {
            events_task_is_scheduled = mp_sched_schedule(MP_OBJ_FROM_PTR(&run_events_scheduled_task_obj), mp_const_none);
        }
        MICROPY_PY_BLUETOOTH_EXIT
        usleep(UART_POLL_INTERVAL_US);
    }

    #else
    DEBUG_printf("hci_poll_thread: async starting\n");

    // In asynchronous (i.e. ringbuffer) mode, we run the BLE stack directly from the thread.
    // This will return false when the stack is shutdown.
    while (mp_bluetooth_hci_poll()) {
        usleep(UART_POLL_INTERVAL_US);
    }

    #endif

    DEBUG_printf("hci_poll_thread: stopped\n");

    return 0;
}

static int configure_uart(void) {

    // If you get an error here it means your compile is detecting the size of the struct
    // such that a empty array is adding 1 to the count - this breaks nimble.
    // for mingw this should be fixed by the compiler flag: -mno-ms-bitfields
    struct ble_hci_ev_command_complete size_test;
    BUILD_BUG_ON(sizeof(size_test) != 4);

    COMMTIMEOUTS timeouts;
    DCB port;

    FillMemory(&port, sizeof(port), 0);
    FillMemory(&timeouts, sizeof(timeouts), 0);

    if (!GetCommState(uart_fd, &port)) {
        DEBUG_printf("Couldn't get com port attributes: %s", GetLastError());
        CloseHandle(uart_fd);
        return -1;
    }

    // Update DCB rate.
    port.BaudRate = 1000000;
    port.fBinary = TRUE;
    port.fParity = FALSE;
    port.fOutxCtsFlow = FALSE;
    port.fOutxDsrFlow = FALSE;
    port.fDtrControl = TRUE;
    port.fDsrSensitivity = FALSE;
    port.fTXContinueOnXoff = FALSE;
    port.fOutX = FALSE;
    port.fInX = FALSE;
    port.fErrorChar = FALSE;
    port.fNull = FALSE;
    port.fRtsControl = TRUE;
    port.fAbortOnError = FALSE;
    port.fDummy2 = FALSE;
    port.wReserved = FALSE;
    port.XonLim = FALSE;
    port.XoffLim = FALSE;
    port.ByteSize = FALSE;
    port.Parity = FALSE;
    port.StopBits = FALSE;
    port.ByteSize = 8;

    // Set new state.
    if (!SetCommState(uart_fd, &port)) {
        // Error in SetCommState. Possibly a problem with the communications
        // port handle or a problem with the DCB structure itself.
        DEBUG_printf("Couldn't set com port attributes: %s", GetLastError());
        CloseHandle(uart_fd);
        return -1;
    }

    // set timeouts on the comm port.
    timeouts.ReadIntervalTimeout = 10;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.ReadTotalTimeoutConstant = 10;
    timeouts.WriteTotalTimeoutMultiplier = 1;
    timeouts.WriteTotalTimeoutConstant = 10;
    if (!SetCommTimeouts(uart_fd, &timeouts)) {
        DEBUG_printf("Couldn't set com port timeout: %s", GetLastError());
        CloseHandle(uart_fd);
        return -1;
    }

    return 0;
}

// HCI UART bindings.
int mp_bluetooth_hci_uart_init(uint32_t port, uint32_t baudrate) {
    (void)port;
    (void)baudrate;

    DEBUG_printf("mp_bluetooth_hci_uart_init (windows)\n");

    if (uart_fd != INVALID_HANDLE_VALUE) {
        DEBUG_printf("mp_bluetooth_hci_uart_init: already active\n");
        return 0;
    }

    char uart_device_name[256] = "\\\\.\\COM3";

    char *path = getenv("MICROPYBTUART");
    if (path != NULL) {
        if (path[0] == '\\') {
            strcpy(uart_device_name, path);

        } else {
            // Keep the "long" com address on the front of uart_device_name
            strcpy(&uart_device_name[4], path);
        }
    }
    DEBUG_printf("mp_bluetooth_hci_uart_init: Using HCI UART: %s\n", uart_device_name);

    uart_fd = CreateFile(uart_device_name,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
    if (uart_fd == INVALID_HANDLE_VALUE) {
        // error opening port; abort
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Couldn't open com port (%s)"), uart_device_name);
        return -1;
    }

    if (configure_uart()) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Couldn't configure com port (%s)"), uart_device_name);
        return -1;
    }

    // Create a thread to run the polling loop.
    mp_thread_windows_init_atomic_section();
    hci_poll_thread_id = CreateThread(NULL, 0, hci_poll_thread, 0, 0, NULL);

    return 0;
}

int mp_bluetooth_hci_uart_deinit(void) {
    DEBUG_printf("mp_bluetooth_hci_uart_deinit\n");

    if (uart_fd == INVALID_HANDLE_VALUE) {
        return 0;
    }

    // Wait for the poll loop to terminate when the state is set to OFF.
    WaitForSingleObject(hci_poll_thread_id, INFINITE);

    // Close the UART.
    CloseHandle(uart_fd);
    uart_fd = INVALID_HANDLE_VALUE;

    return 0;
}

int mp_bluetooth_hci_uart_set_baudrate(uint32_t baudrate) {
    (void)baudrate;
    DEBUG_printf("mp_bluetooth_hci_uart_set_baudrate\n");
    return 0;
}

int mp_bluetooth_hci_uart_readchar(void) {
    if (uart_fd == INVALID_HANDLE_VALUE) {
        return -1;
    }

    uint8_t c = 0;
    DWORD bytes_read;
    if (!ReadFile(uart_fd, &c, 1, &bytes_read, NULL)) {
        DEBUG_printf("mp_bluetooth_hci_uart_readchar Error: %s", GetLastError());
        return -1;
    }

    if (bytes_read) {
        #if DEBUG_HCI_DUMP
        printf("[% 8ld] RX: %02x\n", bytes_read, c);
        #endif
        return c;
    } else {
        return -1;
    }
}

int mp_bluetooth_hci_uart_write(const uint8_t *buf, size_t len) {
    if (uart_fd == INVALID_HANDLE_VALUE) {
        return 0;
    }

    #if DEBUG_HCI_DUMP
    printf("[% 8d] TX: %02x", mp_hal_ticks_ms(), buf[0]);
    for (size_t i = 1; i < len; ++i) {
        printf(":%02x", buf[i]);
    }
    printf("\n");
    #endif

    DWORD bytes_written = 0;
    if (!WriteFile(uart_fd, buf, len, &bytes_written, NULL)) {
        DEBUG_printf("mp_bluetooth_hci_uart_write Error: %s", GetLastError());
        return -1;
    }

    return bytes_written;
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

// Dummy functions to satisfy linker
#include "nimble/nimble_npl.h"
void ble_npl_event_run(struct ble_npl_event *ev) {
    (void)ev;
    DEBUG_printf("ERROR: ble_npl_event_run\n");

}
struct ble_npl_event *ble_npl_eventq_get(struct ble_npl_eventq *evq,
    ble_npl_time_t tmo) {
    (void)evq;
    (void)tmo;
    DEBUG_printf("ERROR: ble_npl_eventq_get\n");
    return NULL;
}


#endif // MICROPY_PY_BLUETOOTH && (MICROPY_BLUETOOTH_NIMBLE || (MICROPY_BLUETOOTH_BTSTACK && MICROPY_BLUETOOTH_BTSTACK_H4))
