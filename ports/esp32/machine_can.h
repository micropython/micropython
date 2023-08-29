/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Musumeci Salvatore
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
#ifndef MICROPY_INCLUDED_ESP32_MACHINE_CAN_H
#define MICROPY_INCLUDED_ESP32_MACHINE_CAN_H

#if MICROPY_PY_MACHINE_CAN
/*
typedef enum {
    TWAI_MODE_NORMAL,      //< Normal operating mode where TWAI controller can send/receive/acknowledge messages
    TWAI_MODE_NO_ACK,      //< Transmission does not require acknowledgment. Use this mode for self testing
    TWAI_MODE_LISTEN_ONLY, //< The TWAI controller will not influence the bus (No transmissions or acknowledgments) but can receive messages
} twai_mode_t;
*/
/*
   - ``NORMAL`` - CAN controller interacts normally on the bus.
   - ``SLEEP`` - CAN controller is asleep in a low power mode. Depending on the
     controller, this may support waking the controller and transitioning to
     ``NORMAL`` mode if CAN traffic is received.
   - ``LOOPBACK`` - A testing mode. The CAN controller is still connected to the
     external bus, but will also receive its own transmitted messages and ignore
     any ACK errors.
   - ``SILENT`` - CAN controller receives messages but does not interact with
     the CAN bus (including sending ACKs, errors, etc.)
   - ``SILENT_LOOPBACK`` - A testing mode that does not require a CAN bus. The
     CAN controller receives its own transmitted messages without interacting
     with the CAN bus at all. The CAN TX and RX pins remain idle.
*/

#define CAN_MODE_SILENT_LOOPBACK (0x10)

typedef enum {
    MODE_NORMAL = TWAI_MODE_NORMAL,
    MODE_SLEEP = -1,
    MODE_LOOPBACK = TWAI_MODE_NO_ACK, // requires wired TX - RX pin connection
    MODE_SILENT = TWAI_MODE_NO_ACK,
    MODE_SILENT_LOOPBACK = TWAI_MODE_NO_ACK | CAN_MODE_SILENT_LOOPBACK,
    MODE_LISTEN_ONLY = TWAI_MODE_LISTEN_ONLY, // esp32 specific
} can_mode_t;

typedef enum {
    FILTER_RAW_SINGLE = 1,
    FILTER_RAW_DUAL,
    FILTER_ADDRESS
} filter_mode_t;

typedef enum {
    RX_STATE_FIFO_EMPTY = 1,
    RX_STATE_MESSAGE_PENDING,
    RX_STATE_FIFO_FULL,
    RX_STATE_FIFO_OVERFLOW,
} rx_state_t;

typedef enum {
    NOT_INITIATED = TWAI_STATE_STOPPED - 1,
    STOPPED = TWAI_STATE_STOPPED,
    RUNNING = TWAI_STATE_RUNNING,
    BUS_OFF = TWAI_STATE_BUS_OFF,
    RECOVERING = TWAI_STATE_RECOVERING,
} state_t;

typedef enum {
    ERROR = -1,
    /*
    ERROR_ACTIVE = TWAI_ERROR_ACTIVE,
    ERROR_WARNING = TWAI_ERROR_WARNING,
    ERROR_PASSIVE = TWAI_ERROR_PASSIVE,
    ERROR_BUS_OFF = TWAI_ERROR_BUS_OFF,
    */
} error_state_t;

typedef enum {
    RTR = 1,
    EXTENDED_ID,
    FD_F,
    BRS,
} message_flags_t;

typedef enum {
    CRC = 1,
    FORM,
    OVERRUN,
    ESI,
} recv_errors_t;

typedef enum {
    ARB = 1,
    NACK,
    ERR,
} send_errors_t;

typedef struct {
    mp_obj_base_t base;

    twai_timing_config_t t_config;
    twai_filter_config_t f_config;
    twai_general_config_t g_config;
    uint32_t bitrate; // bit/s

    mp_obj_t rx_callback;
    mp_obj_t tx_callback;
    TaskHandle_t irq_handler;
    byte rx_state;
    bool extframe : 1;
    bool loopback : 1;
    byte last_tx_success : 1;
    byte bus_recovery_success : 1;
    uint16_t num_error_warning; // FIXME: populate this value somewhere
    uint16_t num_error_passive;
    uint16_t num_bus_off;
    twai_handle_t handle;
    twai_status_info_t status;
} esp32_can_obj_t;

extern const mp_obj_type_t machine_can_type;

#endif // MICROPY_PY_MACHINE_CAN

#endif // MICROPY_INCLUDED_ESP32_MACHINE_CAN_H
