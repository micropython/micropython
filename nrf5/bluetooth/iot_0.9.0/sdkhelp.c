/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#include <stdbool.h>

#include "softdevice_handler.h"
#include "app_trace.h"
#include "app_timer_appsh.h"
#include "ble_advdata.h"
#include "ble_srv_common.h"
#include "ble_ipsp.h"
#include "ble_6lowpan.h"
#include "mem_manager.h"
#include "app_trace.h"
#include "lwip/init.h"
#include "lwip/inet6.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"
#include "lwip/timers.h"
#include "nrf_platform_port.h"
#include "app_util_platform.h"
#include "ble_gap.h"
#include "sdkhelp.h"
#include "mpconfigport.h"
#include "app_timer_appsh.h"
#include "iot_timer.h"

#include "iot_defines.h"

eui64_t eui64_local_iid;

bool m_interface_up = false;

void nrf_driver_interface_up(void)
{   
    // sys_check_timeouts();
	m_interface_up = true;
}

void nrf_driver_interface_down(void)
{
	m_interface_up = false;
}

#define DEVICE_NAME                "MPY_IPv6"
#define APP_ADV_TIMEOUT            (0)   // disable timeout
#define APP_ADV_ADV_INTERVAL       MSEC_TO_UNITS(333, UNIT_0_625_MS)
#define IOT_TIMER_RESOLUTION_IN_MS 100
#define LWIP_SYS_TICK_MS           100
#define APP_TIMER_PRESCALER        31 // Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS       1
#define APP_TIMER_OP_QUEUE_SIZE    5

APP_TIMER_DEF(m_iot_timer_tick_src_id);

static ble_gap_adv_params_t m_adv_params;
static ble_gap_addr_t       m_local_ble_addr;

static void app_lwip_time_tick(iot_timer_time_in_ms_t wall_clock_value)
{
    sys_check_timeouts();
}

static void iot_timer_init(void)
{
    uint32_t err_code;

    static const iot_timer_client_t list_of_clients[] =
    {
        {app_lwip_time_tick, LWIP_SYS_TICK_MS}
    };

    // The list of IoT Timer clients is declared as a constant.
    static const iot_timer_clients_list_t iot_timer_clients =
    {
        (sizeof(list_of_clients) / sizeof(iot_timer_client_t)),
        &(list_of_clients[0]),
    };

    // Passing the list of clients to the IoT Timer module.
    err_code = iot_timer_client_list_set(&iot_timer_clients);
    APP_ERROR_CHECK(err_code);

    // Starting the app timer instance that is the tick source for the IoT Timer.
    err_code = app_timer_start(m_iot_timer_tick_src_id, \
                               APP_TIMER_TICKS(IOT_TIMER_RESOLUTION_IN_MS, APP_TIMER_PRESCALER), \
                               NULL);
    APP_ERROR_CHECK(err_code);
}

static void iot_timer_tick_callback(void * p_context)
{
    UNUSED_VARIABLE(p_context);
    uint32_t err_code = iot_timer_update();
    APP_ERROR_CHECK(err_code);
}

static void timers_init(void)
{
    uint32_t err_code;

    // Initialize timer module, making it use the scheduler
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    // Create a sys timer.
    err_code = app_timer_create(&m_iot_timer_tick_src_id,
                                APP_TIMER_MODE_REPEATED,
                                iot_timer_tick_callback);
    APP_ERROR_CHECK(err_code);
}

static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            break;
        case BLE_GAP_EVT_DISCONNECTED:
        	sd_ble_gap_adv_start(&m_adv_params);
            break;
        default:
            break;
    }
}

static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_ipsp_evt_handler(p_ble_evt);
    on_ble_evt(p_ble_evt);
}

void transport_init(void) {

	// if interface is already up, return
	if (m_interface_up) {
        return;
    }

    uint32_t err_code;

    timers_init();

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, false);
    printf("Softdevice init done\n");

    // Enable BLE stack.
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0x00, sizeof(ble_enable_params));
    ble_enable_params.gatts_enable_params.attr_tab_size = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
    ble_enable_params.gatts_enable_params.service_changed = false;
    err_code = sd_ble_enable(&ble_enable_params);
    printf("Softdevice enable:" UINT_FMT "\n", (uint16_t)err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    printf("Softdevice evt handler set:" UINT_FMT "\n", (uint16_t)err_code);
    APP_ERROR_CHECK(err_code);

    ble_advdata_t           advdata;
    uint8_t                 flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    printf("Device name set:" UINT_FMT "\n", (uint16_t)err_code);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_address_get(&m_local_ble_addr);
    APP_ERROR_CHECK(err_code);
    printf("GAP address get:" UINT_FMT "\n", (uint16_t)err_code);

    m_local_ble_addr.addr[5]   = 0x00;
    m_local_ble_addr.addr_type = BLE_GAP_ADDR_TYPE_PUBLIC;

    err_code = sd_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_NONE, &m_local_ble_addr);
    APP_ERROR_CHECK(err_code);
    printf("GAP address set:" UINT_FMT "\n", (uint16_t)err_code);
    IPV6_EUI64_CREATE_FROM_EUI48(eui64_local_iid.identifier,
                                 m_local_ble_addr.addr,
                                 m_local_ble_addr.addr_type);

    ble_uuid_t adv_uuids[] =
    {
        {BLE_UUID_IPSP_SERVICE, BLE_UUID_TYPE_BLE}
    };

    // build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.flags                   = flags;
    advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = adv_uuids;

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);
    printf("Adv data set:" UINT_FMT "\n", (uint16_t)err_code);
    // initialize advertising parameters (used when starting advertising)
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    m_adv_params.p_peer_addr = NULL;                             // undirected advertisement
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = APP_ADV_ADV_INTERVAL;
    m_adv_params.timeout     = APP_ADV_TIMEOUT;

    // initialize memory manager
    err_code = nrf_mem_init();
    APP_ERROR_CHECK(err_code);
    printf("mem init:" UINT_FMT "\n", (uint16_t)err_code);
    // initialize lwip stack driver
    err_code = nrf_driver_init();
    APP_ERROR_CHECK(err_code);
    printf("driver init:" UINT_FMT "\n", (uint16_t)err_code);
    // initialize lwip stack
    lwip_init();
    printf("lwip init:" UINT_FMT "\n", (uint16_t)err_code);

    iot_timer_init();

    printf("Starting adv:" UINT_FMT "\n", (uint16_t)err_code);
    err_code = sd_ble_gap_adv_start(&m_adv_params);

    while (!m_interface_up) {
        ;
    }
}

