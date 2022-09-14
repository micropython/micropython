/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK

#include "lib/btstack/src/btstack.h"
#include "extmod/mpbthci.h"
#include "extmod/btstack/btstack_hci_uart.h"
#include "extmod/btstack/modbluetooth_btstack.h"
#include "mpbthciport.h"

void mp_bluetooth_hci_poll_in_ms(uint32_t ms);

static btstack_linked_list_t mp_btstack_runloop_timers;

static void mp_btstack_runloop_init(void) {
    mp_btstack_runloop_timers = NULL;
}

static void mp_btstack_runloop_set_timer(btstack_timer_source_t *tim, uint32_t timeout_ms) {
    tim->timeout = mp_hal_ticks_ms() + timeout_ms + 1;
}

static void mp_btstack_runloop_add_timer(btstack_timer_source_t *tim) {
    btstack_linked_item_t **node = &mp_btstack_runloop_timers;
    for (; *node; node = &(*node)->next) {
        btstack_timer_source_t *node_tim = (btstack_timer_source_t *)*node;
        if (node_tim == tim) {
            // Timer is already in the list, don't add it.
            return;
        }
        int32_t delta = btstack_time_delta(tim->timeout, node_tim->timeout);
        if (delta < 0) {
            // Found sorted location in list.
            break;
        }
    }

    // Insert timer into list in sorted location.
    tim->item.next = *node;
    *node = &tim->item;

    // Reschedule the HCI poll if this timer is at the head of the list.
    if (mp_btstack_runloop_timers == &tim->item) {
        int32_t delta_ms = btstack_time_delta(tim->timeout, mp_hal_ticks_ms());
        mp_bluetooth_hci_poll_in_ms(delta_ms);
    }
}

static bool mp_btstack_runloop_remove_timer(btstack_timer_source_t *tim) {
    return btstack_linked_list_remove(&mp_btstack_runloop_timers, (btstack_linked_item_t *)tim);
}

static void mp_btstack_runloop_execute(void) {
    // Should not be called.
}

static void mp_btstack_runloop_dump_timer(void) {
    // Not implemented/needed.
}

static uint32_t mp_btstack_runloop_get_time_ms(void) {
    return mp_hal_ticks_ms();
}

static const btstack_run_loop_t mp_btstack_runloop_stm32 = {
    &mp_btstack_runloop_init,
    NULL, // add_data_source,
    NULL, // remove_data_source,
    NULL, // enable_data_source_callbacks,
    NULL, // disable_data_source_callbacks,
    &mp_btstack_runloop_set_timer,
    &mp_btstack_runloop_add_timer,
    &mp_btstack_runloop_remove_timer,
    &mp_btstack_runloop_execute,
    &mp_btstack_runloop_dump_timer,
    &mp_btstack_runloop_get_time_ms,
};


// #if defined(STM32WB)

// /******************************************************************************/
// // HCI over IPCC

// #include "rfcore.h"

// static void (*transport_packet_handler)(uint8_t packet_type, uint8_t *packet, uint16_t size);
// static int hci_acl_can_send_now;


// STATIC void rfcore_transport_init(const void *transport_config){
//     rfcore_init();
// }

// STATIC int rfcore_transport_open(void){
//     return 0;
// }

// STATIC int rfcore_transport_close(void){
//     return 0;
// }


// STATIC void rfcore_transport_register_packet_handler(void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size)){
//     transport_packet_handler = handler;
// }

// STATIC int rfcore_transport_can_send_packet_now(uint8_t packet_type) {
//     // if (cpu2_state != CPU2_STATE_READY) return 0;
//     // switch (packet_type)
//     // {
//     //     case HCI_COMMAND_DATA_PACKET:
//     //         return 1;

//     //     case HCI_ACL_DATA_PACKET:
//     //         return hci_acl_can_send_now;
//     // }
//     return 1;
// }

// STATIC int rfcore_transport_send_packet(uint8_t packet_type, uint8_t *packet, int size){
// 	TL_CmdPacket_t *ble_cmd_buff = &BleCmdBuffer;

//     switch (packet_type){
//         case HCI_COMMAND_DATA_PACKET:
//             ble_cmd_buff->cmdserial.type = packet_type;
//             ble_cmd_buff->cmdserial.cmd.plen = size;
//             memcpy((void *)&ble_cmd_buff->cmdserial.cmd, packet, size);
//             TL_BLE_SendCmd(NULL, 0);
//             transport_notify_packet_send();
//             break;

//         case HCI_ACL_DATA_PACKET:
//             hci_acl_can_send_now = 0;
//             ((TL_AclDataPacket_t *)HciAclDataBuffer)->AclDataSerial.type = packet_type;
//             memcpy((void *)&(((TL_AclDataPacket_t *)HciAclDataBuffer)->AclDataSerial.handle),packet, size);
//             TL_BLE_SendAclData(NULL, 0);
//             transport_notify_packet_send();
//             break;

//         default:
//             transport_send_hardware_error(0x01);  // invalid HCI packet
//             break;
//     }
//     return 0;
// }

// STATIC const hci_transport_t hci_wb55_transport = {
//     "stm32wb-vhci",
//     &rfcore_transport_init,
//     &rfcore_transport_open,
//     &rfcore_transport_close,
//     &rfcore_transport_register_packet_handler,
//     &rfcore_transport_can_send_packet_now,
//     &rfcore_transport_send_packet,
//     NULL, // set baud rate
//     NULL, // reset link
//     NULL, // set SCO config
// };

// void mp_bluetooth_btstack_port_init(void) {
//     btstack_run_loop_init(&mp_btstack_runloop_stm32);

//     // hci_dump_open(NULL, HCI_DUMP_STDOUT);
//     hci_init(&hci_wb55_transport, NULL);
// }

// #else

/******************************************************************************/
// HCI over UART

#ifndef MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY
#define MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY MICROPY_HW_BLE_UART_BAUDRATE
#endif

STATIC const hci_transport_config_uart_t hci_transport_config_uart = {
    HCI_TRANSPORT_CONFIG_UART,
    MICROPY_HW_BLE_UART_BAUDRATE,
    MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY,
    0,
    NULL,
};

void mp_bluetooth_hci_poll(void) {
    if (mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_OFF) {
        return;
    }

    // Process UART data.
    if (mp_bluetooth_btstack_state != MP_BLUETOOTH_BTSTACK_STATE_HALTING) {
        mp_bluetooth_btstack_hci_uart_process();
    }

    // Process any BTstack timers.
    while (mp_btstack_runloop_timers != NULL) {
        btstack_timer_source_t *tim = (btstack_timer_source_t *)mp_btstack_runloop_timers;
        int32_t delta_ms = btstack_time_delta(tim->timeout, mp_hal_ticks_ms());
        if (delta_ms > 0) {
            // Timer has not expired yet, reschedule HCI poll for this timer.
            mp_bluetooth_hci_poll_in_ms(delta_ms);
            break;
        }
        btstack_linked_list_pop(&mp_btstack_runloop_timers);
        tim->process(tim);
    }
}

void mp_bluetooth_btstack_port_init(void) {
    static bool run_loop_init = false;
    if (!run_loop_init) {
        run_loop_init = true;
        btstack_run_loop_init(&mp_btstack_runloop_stm32);
    } else {
        mp_btstack_runloop_stm32.init();
    }

    // hci_dump_open(NULL, HCI_DUMP_STDOUT);
    const hci_transport_t *transport = hci_transport_h4_instance(&mp_bluetooth_btstack_hci_uart_block);
    hci_init(transport, &hci_transport_config_uart);

    #ifdef MICROPY_HW_BLE_BTSTACK_CHIPSET_INSTANCE
    hci_set_chipset(MICROPY_HW_BLE_BTSTACK_CHIPSET_INSTANCE);
    #endif
}

void mp_bluetooth_btstack_port_deinit(void) {
    hci_power_control(HCI_POWER_OFF);
    hci_close();
}

void mp_bluetooth_btstack_port_start(void) {
    hci_power_control(HCI_POWER_ON);
}


#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK
