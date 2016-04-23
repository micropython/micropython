/**
 ******************************************************************************
 * @file    hal_dynalib_btstack.h
 * @authors mat
 * @date    04 March 2015
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
#ifndef HAL_DYNALIB_BTSTACK_H
#define HAL_DYNALIB_BTSTACK_H

#if PLATFORM_ID == 88 // Duo

#include "dynalib.h"
#include "usb_config_hal.h"

#ifdef DYNALIB_EXPORT
#include "btstack_hal.h"
#endif

// WARNING
// The order of functions must not be changed or older applications will break
// when used with newer system firmware.
// Function signatures shouldn't be changed other than changing pointer types.
// New HAL functions must be added to the end of this list.
// GNINRAW

DYNALIB_BEGIN(hal_btstack)

DYNALIB_FN(hal_btstack,hal_btstack_init)
DYNALIB_FN(hal_btstack,hal_btstack_deInit)
DYNALIB_FN(hal_btstack,hal_btstack_loop_execute)

DYNALIB_FN(hal_btstack,hal_btstack_setTimer)
DYNALIB_FN(hal_btstack,hal_btstack_setTimerHandler)
DYNALIB_FN(hal_btstack,hal_btstack_addTimer)
DYNALIB_FN(hal_btstack,hal_btstack_removeTimer)
DYNALIB_FN(hal_btstack,hal_btstack_getTimeMs)

DYNALIB_FN(hal_btstack,hal_btstack_Log_info)
DYNALIB_FN(hal_btstack,hal_btstack_error_info)
DYNALIB_FN(hal_btstack,hal_btstack_enable_packet_info)


DYNALIB_FN(hal_btstack,hal_btstack_setRandomAddressMode)
DYNALIB_FN(hal_btstack,hal_btstack_setRandomAddr)
DYNALIB_FN(hal_btstack,hal_btstack_setPublicBdAddr)
DYNALIB_FN(hal_btstack,hal_btstack_setLocalName)
DYNALIB_FN(hal_btstack,hal_btstack_setAdvParams)
DYNALIB_FN(hal_btstack,hal_btstack_setAdvData)
DYNALIB_FN(hal_btstack,hal_btstack_startAdvertising)
DYNALIB_FN(hal_btstack,hal_btstack_stopAdvertising)

DYNALIB_FN(hal_btstack,hal_btstack_setConnectedCallback)
DYNALIB_FN(hal_btstack,hal_btstack_setDisconnectedCallback)

DYNALIB_FN(hal_btstack,hal_btstack_disconnect)
DYNALIB_FN(hal_btstack,hal_btstack_connect)

DYNALIB_FN(hal_btstack,hal_btstack_setConnParamsRange)

DYNALIB_FN(hal_btstack,hal_btstack_startScanning)
DYNALIB_FN(hal_btstack,hal_btstack_stopScanning)

DYNALIB_FN(hal_btstack,hal_btstack_setScanParams)
DYNALIB_FN(hal_btstack,hal_btstack_setBLEAdvertisementCallback)

DYNALIB_FN(hal_btstack,hal_btstack_attServerCanSend)
DYNALIB_FN(hal_btstack,hal_btstack_attServerSendNotify)
DYNALIB_FN(hal_btstack,hal_btstack_attServerSendIndicate)

DYNALIB_FN(hal_btstack,hal_btstack_setGattCharsRead)
DYNALIB_FN(hal_btstack,hal_btstack_setGattCharsWrite)

DYNALIB_FN(hal_btstack,hal_btstack_addServiceUUID16bits)
DYNALIB_FN(hal_btstack,hal_btstack_addServiceUUID128bits)
DYNALIB_FN(hal_btstack,hal_btstack_addCharsUUID16bits)
DYNALIB_FN(hal_btstack,hal_btstack_addCharsUUID128bits)
DYNALIB_FN(hal_btstack,hal_btstack_addCharsDynamicUUID16bits)
DYNALIB_FN(hal_btstack,hal_btstack_addCharsDynamicUUID128bits)


DYNALIB_FN(hal_btstack,hal_btstack_setGattServiceDiscoveredCallback)
DYNALIB_FN(hal_btstack,hal_btstack_setGattCharsDiscoveredCallback)
DYNALIB_FN(hal_btstack,hal_btstack_setGattCharsDescriptorsDiscoveredCallback)
DYNALIB_FN(hal_btstack,hal_btstack_setGattCharacteristicReadCallback)
DYNALIB_FN(hal_btstack,hal_btstack_setGattCharacteristicWrittenCallback)
DYNALIB_FN(hal_btstack,hal_btstack_setGattCharsDescriptorReadCallback)
DYNALIB_FN(hal_btstack,hal_btstack_setGattCharsDescriptorWrittenCallback)

DYNALIB_FN(hal_btstack,hal_btstack_setGattWriteClientCharacteristicConfigCallback)
DYNALIB_FN(hal_btstack,hal_btstack_setGattNotifyUpdateCallback)
DYNALIB_FN(hal_btstack,hal_btstack_setGattIndicateUpdateCallback)

DYNALIB_FN(hal_btstack,hal_btstack_discoverPrimaryServices)
DYNALIB_FN(hal_btstack,hal_btstack_discoverPrimaryServicesByUUID16)
DYNALIB_FN(hal_btstack,hal_btstack_discoverPrimaryServicesByUUID128)

DYNALIB_FN(hal_btstack,hal_btstack_discoverCharsForService)
DYNALIB_FN(hal_btstack,hal_btstack_discoverCharsForHandleRangeByUUID16)
DYNALIB_FN(hal_btstack,hal_btstack_discoverCharsForHandleRangeByUUID128)
DYNALIB_FN(hal_btstack,hal_btstack_discoverCharsForServiceByUUID16)
DYNALIB_FN(hal_btstack,hal_btstack_discoverCharsForServiceByUUID128)

DYNALIB_FN(hal_btstack,hal_btstack_discoverCharsDescriptors)

DYNALIB_FN(hal_btstack,hal_btstack_readValueOfCharacteristic)
DYNALIB_FN(hal_btstack,hal_btstack_readValueOfCharacteristicUsingValueHandle)
DYNALIB_FN(hal_btstack,hal_btstack_readValueOfCharacteristicByUUID16)
DYNALIB_FN(hal_btstack,hal_btstack_readValueOfCharacteristicByUUID128)
DYNALIB_FN(hal_btstack,hal_btstack_readLongValueOfCharacteristic)
DYNALIB_FN(hal_btstack,hal_btstack_readLongValueOfCharacteristicUsingValueHandle)
DYNALIB_FN(hal_btstack,hal_btstack_readLongValueOfCharacteristicUsingValueHandleWithOffset)

DYNALIB_FN(hal_btstack,hal_btstack_writeValueOfChracteristicWithoutResponse)
DYNALIB_FN(hal_btstack,hal_btstack_writeValueOfCharacteristic)
DYNALIB_FN(hal_btstack,hal_btstack_writeLongValueOfCharacteristic)
DYNALIB_FN(hal_btstack,hal_btstack_writeLongValueOfCharacteristicWithOffset)

DYNALIB_FN(hal_btstack,hal_btstack_readCharacteristicDescriptor)
DYNALIB_FN(hal_btstack,hal_btstack_readCharacteristicDescriptorUsingDescriptorHandle)
DYNALIB_FN(hal_btstack,hal_btstack_readLongCharacteristicDescriptor)
DYNALIB_FN(hal_btstack,hal_btstack_readLongCharacteristicDescriptorUsingDescriptorHandle)
DYNALIB_FN(hal_btstack,hal_btstack_readLongCharacteristicDescriptorUsingDescriptorHandleWithOffset)

DYNALIB_FN(hal_btstack,hal_btstack_writeCharacteristicDescriptor)
DYNALIB_FN(hal_btstack,hal_btstack_writeCharacteristicDescriptorUsingDescriptorHandle)
DYNALIB_FN(hal_btstack,hal_btstack_writeLongCharacteristicDescriptor)
DYNALIB_FN(hal_btstack,hal_btstack_writeLongCharacteristicDescriptorUsingDescriptorHandle)
DYNALIB_FN(hal_btstack,hal_btstack_writeLongCharacteristicDescriptorUsingDescriptorHandleWithOffset)

DYNALIB_FN(hal_btstack,hal_btstack_WriteClientCharacteristicConfiguration)
DYNALIB_FN(hal_btstack,hal_btstack_listenForCharacteristicValueUpdates)


DYNALIB_END(hal_btstack)

#endif

#endif /* HAL_DYNALIB_BTSTACK_H */
