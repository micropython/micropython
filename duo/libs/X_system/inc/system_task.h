/**
 ******************************************************************************
 * @file    spark_wiring_wlan.h
 * @author  Satish Nair and Zachary Crockett
 * @version V1.0.0
 * @date    13-March-2013
 * @brief   Header for spark_wiring_wlan.c module
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPARK_WLAN_H
#define __SPARK_WLAN_H

#include "socket_hal.h"
#include "system_cloud.h"
#include "wlan_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t HAL_NET_SetNetWatchDog(uint32_t timeOutInuS);
void Network_Setup(bool threaded);

/**
 * Run background processing. This function should be called as often as possible by user code.
 * @param force_events when true, runs cloud event pump in addition to maintaining the wifi and cloud connection.
 */
void Spark_Idle_Events(bool force_events);
inline void Spark_Idle() { Spark_Idle_Events(false); }

/**
 * The old method
 */
void SPARK_WLAN_Loop(void) __attribute__ ((deprecated("Please use Particle.process() instead.")));
inline void SPARK_WLAN_Loop(void) { spark_process(); }

void disconnect_cloud();

extern volatile uint32_t TimingFlashUpdateTimeout;

extern volatile uint8_t SPARK_WLAN_RESET;
extern volatile uint8_t SPARK_WLAN_SLEEP;
extern volatile uint8_t SPARK_WLAN_STARTED;
extern volatile uint8_t SPARK_CLOUD_CONNECT;
extern volatile uint8_t SPARK_CLOUD_SOCKETED;
extern volatile uint8_t SPARK_CLOUD_CONNECTED;
extern volatile uint8_t SPARK_FLASH_UPDATE;
extern volatile uint8_t SPARK_LED_FADE;

extern volatile uint8_t Spark_Error_Count;
extern volatile uint8_t Cloud_Handshake_Error_Count;
extern volatile uint8_t SYSTEM_POWEROFF;

extern volatile system_tick_t spark_loop_total_millis;

void system_delay_ms(unsigned long ms, bool no_background_loop);

/**
 * Determines the backoff period after a number of failed connections.
 */
unsigned backoff_period(unsigned connection_attempts);

/**
 * This is for internal testing. Do not call this function since it is not
 * guaranteed to be preserved or backwards compatible between releases.
 */
void* system_internal(int item, void* reserved);


#ifdef __cplusplus
}
#endif

#endif  /*__SPARK_WLAN_H*/
