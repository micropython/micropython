/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

#ifndef MICROPY_INCLUDED_PSOC_EDGE_MACHINE_IPC_H
#define MICROPY_INCLUDED_PSOC_EDGE_MACHINE_IPC_H

#include "py/obj.h"
#include "cy_ipc_pipe.h"
#include "ipc_communication.h"
#include "shared/runtime/mpirq.h"

/* IPC Pipe Endpoint-1 config */
#define CY_IPC_CYPIPE_CHAN_MASK_EP1     CY_IPC_CH_MASK(CY_IPC_CHAN_CYPIPE_EP1)
#define CY_IPC_CYPIPE_INTR_MASK_EP1     CY_IPC_INTR_MASK(CY_IPC_INTR_CYPIPE_EP1)
#define CY_IPC_INTR_CYPIPE_PRIOR_EP1    (1UL)
#define CY_IPC_INTR_CYPIPE_MUX_EP1      (CY_IPC0_INTR_MUX(CY_IPC_INTR_CYPIPE_EP1))
#define CM33_IPC_PIPE_EP_ADDR           (1UL)
#define CM33_IPC_PIPE_CLIENT_ID         (3UL) // Pre-defined

/* IPC Pipe Endpoint-2 config */
#define CY_IPC_CYPIPE_CHAN_MASK_EP2     CY_IPC_CH_MASK(CY_IPC_CHAN_CYPIPE_EP2)
#define CY_IPC_CYPIPE_INTR_MASK_EP2     CY_IPC_INTR_MASK(CY_IPC_INTR_CYPIPE_EP2)
#define CY_IPC_INTR_CYPIPE_PRIOR_EP2    (1UL)
#define CY_IPC_INTR_CYPIPE_MUX_EP2      (CY_IPC0_INTR_MUX(CY_IPC_INTR_CYPIPE_EP2))
#define CM55_IPC_PIPE_EP_ADDR           (2UL)
#define CM55_IPC_PIPE_CLIENT_ID         (5UL)


// Fix IPC commands for now to start and stop as 0x82 and 0x83, can be extended in the future as needed
// ToDo: How can this be generic and not hardcoded?
#define IPC_CMD_START                   (0x82)
#define IPC_CMD_STOP                    (0x83)

/* Sentinel value meaning a slot in sender_clients_arr is free (not yet registered) */
#define IPC_CLIENT_ID_UNREGISTERED      (0xFFU)

typedef struct _machine_ipc_client_obj_t {
    mp_irq_obj_t base;   // handler, parent (self), ishard
    uint8_t client_id;   // IPC_CLIENT_ID_UNREGISTERED means slot is free
    uint8_t ep_id;
    uint32_t ep_addr;
    uint8_t last_cmd;    // last received command (set in ISR before mp_irq_handler)
    uint32_t last_value; // last received value
} machine_ipc_client_obj_t;

// Array of registered clients, max IPC_MAX_CLIENTS_PER_EP entries
machine_ipc_client_obj_t sender_clients_arr[8];

// A singleton IPC instance should be created for each core and clients can register to this instance to send messages to the other core. For receiving messages, clients will register their callbacks to the endpoint structure which will be used by the IPC ISR to schedule the appropriate callback in the MicroPython VM via the soft-IRQ scheduler (mp_irq_handler / mp_sched_schedule) when a message is received from the other core.
typedef struct _machine_ipc_obj_t {
    mp_obj_base_t base;
    machine_ipc_client_obj_t *sender_endpoint;
    // ipc_receiver_endpoint_t *receiver_endpoint; // Pointer to receiver information structure
    uint8_t target_core;
    uint8_t src_core;
} machine_ipc_obj_t; // Endpoint same as core

typedef struct
{
    uint8_t client_id;
    uint16_t intr_mask;
    uint8_t cmd;
    uint32_t value;
} ipc_msg_t;


bool is_cm55_enabled;
extern const mp_obj_type_t machine_ipc_client_type;
extern const mp_obj_type_t machine_ipc_type;
void machine_ipc_deinit_all(void);

#endif // MICROPY_INCLUDED_PSOC_EDGE_MACHINE_IPC_H
