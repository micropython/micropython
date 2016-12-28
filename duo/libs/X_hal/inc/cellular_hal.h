/*
 ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
  ******************************************************************************
 */

#ifndef CELLULAR_HAL_H
#define	CELLULAR_HAL_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "net_hal.h"
#include "inet_hal.h"
#include "system_tick_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int cellular_result_t;

typedef int (*_CALLBACKPTR_MDM)(int type, const char* buf, int len, void* param);

/**
 * Power on and initialize the cellular module,
 * if USART3 not initialized, will be done on first call.
 */
cellular_result_t  cellular_on(void* reserved);

cellular_result_t  cellular_init(void* reserved);

/**
 * Power off the cellular module.
 */
cellular_result_t  cellular_off(void* reserved);

#ifdef __cplusplus
// Todo - is storing raw string pointers correct here? These will only be valid
// If they are stored as constants in the application.
struct CellularCredentials
{
    uint16_t size;
    const char* apn = "";
    const char* username = "";
    const char* password = "";

    CellularCredentials()
    {
        size = sizeof(*this);
    }
};
#else
typedef struct CellularCredentials CellularCredentials;
#endif

/**
 * Wait for the cellular module to register on the GSM network.
 */
cellular_result_t  cellular_register(void* reserved);

/**
 * Activate the PDP context
 */
cellular_result_t  cellular_pdp_activate(CellularCredentials* connect, void* reserved);

/**
 * Deactivate the PDP context
 */
cellular_result_t  cellular_pdp_deactivate(void* reserved);

/**
 * Perform a GPRS attach.
 */
cellular_result_t  cellular_gprs_attach(CellularCredentials* connect, void* reserved);

/**
 * Perform a GPRS detach.
 */
cellular_result_t  cellular_gprs_detach(void* reserved);

/**
 * Fetch the ip configuration.
 */
cellular_result_t  cellular_fetch_ipconfig(WLanConfig* config, void* reserved);

#ifdef __cplusplus
struct CellularDevice
{
    uint16_t size;
    char iccid[21];
    char imei[16];

    CellularDevice()
    {
        memset(this, 0, sizeof(*this));
        size = sizeof(*this);
    }
};
#else
typedef struct CellularDevice CellularDevice;
#endif

/**
 * Retrieve cellular module info, must be initialized first.
 */
cellular_result_t cellular_device_info(CellularDevice* device, void* reserved);

/**
 * Set cellular connection parameters
 */
cellular_result_t cellular_credentials_set(const char* apn, const char* username, const char* password, void* reserved);

/**
 * Get cellular connection parameters
 */
CellularCredentials* cellular_credentials_get(void* reserved);

bool cellular_sim_ready(void* reserved);

/**
 * Attempts to stop/resume the cellular modem from performing AT operations.
 * Called from another thread or ISR context.
 *
 * @param cancel: true to cancel AT operations, false will resume AT operations.
 *        calledFromISR: true if called from ISR, false if called from main system thread.
 *        reserved: pass NULL. Allows future expansion.
 */
void cellular_cancel(bool cancel, bool calledFromISR, void* reserved);

#ifdef __cplusplus
struct CellularSignalHal
{
    int rssi = 0;
    int qual = 0;
};
#else
typedef struct CellularSignalHal CellularSignalHal;
#endif

/**
 * Retrieve cellular signal strength info
 */
cellular_result_t cellular_signal(CellularSignalHal &signal, void* reserved);

/**
 * Send an AT command and wait for response, optionally specify a callback function to parse the results
 */
cellular_result_t cellular_command(_CALLBACKPTR_MDM cb, void* param,
                         system_tick_t timeout_ms, const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif	/* CELLULAR_HAL_H */

